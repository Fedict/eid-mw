#!/bin/bash

# Build one native, system-wide product installer from the existing components.
set -euo pipefail

SCRIPT_DIR="$(cd "$(dirname "${BASH_SOURCE[0]}")" && pwd)"
REPO_ROOT="$(cd "$SCRIPT_DIR/../.." && pwd)"
MIDDLEWARE_PKG="$REPO_ROOT/installers/eid-mw/mac/exports/export_eidmw/eID-Quickinstaller-signed.pkg"
ESIGN_PKG="$REPO_ROOT/../esign-be-app-macos/install/dist/1.esign-install.pkg"
ESIGN_UNINSTALL_PKG=""
OUTPUT=""
PKG_OUTPUT=""
SIGN_IDENTITY="${EIDMW_CODESIGN_IDENTITY_APP:-Developer ID Application: Zetes Sa (EU27N85PBZ)}"
INSTALLER_IDENTITY="${EIDMW_CODESIGN_IDENTITY_INST:-Developer ID Installer: Zetes Sa (EU27N85PBZ)}"
NOTARY_PROFILE="${EIDMW_NOTARY_PROFILE:-notarytool}"
NOTARIZE_MODE=auto
SIGN=1

usage() {
    cat <<'USAGE'
Usage: make-combined-dmg.sh [options]

  --middleware-pkg PATH   Signed eID middleware product package
  --esign-pkg PATH        Signed system-wide esign product package
  --esign-uninstall-pkg PATH Signed system-wide uninstaller (default: beside esign)
  --output PATH           DMG (default: scripts/mac/eID-Quickinstaller-<mw-version>.dmg)
  --pkg-output PATH       Combined PKG (default: beside the DMG, with the same stem)
  --sign-identity NAME    Developer ID Application identity for the DMG
  --installer-identity NAME Developer ID Installer identity for the combined PKG
  --notarize              Notarize, staple and validate (default for signed builds)
  --skip-notarization     Skip notarization for local inspection
  --keychain-profile NAME Notarytool profile (default: notarytool)
  --unsigned             Skip output signing and notarization for local inspection
  --help                 Show this help

All input product packages must be signed. Existing outputs are never overwritten.
Rebuild esign's packages before assembly; old home-only packages are rejected.
USAGE
}

fail() { echo "Error: $*" >&2; exit 1; }
while [[ $# -gt 0 ]]; do
    case "$1" in
        --middleware-pkg|--esign-pkg|--esign-uninstall-pkg|--output|--pkg-output|--sign-identity|--installer-identity|--keychain-profile)
            [[ $# -ge 2 && -n "$2" ]] || fail "Missing value for $1"
            case "$1" in
                --middleware-pkg) MIDDLEWARE_PKG="$2" ;;
                --esign-pkg) ESIGN_PKG="$2" ;;
                --esign-uninstall-pkg) ESIGN_UNINSTALL_PKG="$2" ;;
                --output) OUTPUT="$2" ;;
                --pkg-output) PKG_OUTPUT="$2" ;;
                --sign-identity) SIGN_IDENTITY="$2" ;;
                --installer-identity) INSTALLER_IDENTITY="$2" ;;
                --keychain-profile) NOTARY_PROFILE="$2" ;;
            esac
            shift 2 ;;
        --notarize) NOTARIZE_MODE=on; shift ;;
        --skip-notarization) NOTARIZE_MODE=off; shift ;;
        --unsigned) SIGN=0; shift ;;
        --help|-h) usage; exit 0 ;;
        *) fail "Unknown option: $1" ;;
    esac
done
case "$NOTARIZE_MODE" in
    auto) NOTARIZE="$SIGN" ;;
    on) [[ "$SIGN" -eq 1 ]] || fail "Notarization requires signing"; NOTARIZE=1 ;;
    off) NOTARIZE=0 ;;
esac
ESIGN_UNINSTALL_PKG="${ESIGN_UNINSTALL_PKG:-$(dirname "$ESIGN_PKG")/esign-uninstall.pkg}"
for package in "$MIDDLEWARE_PKG" "$ESIGN_PKG" "$ESIGN_UNINSTALL_PKG"; do
    [[ -f "$package" && -r "$package" ]] || fail "Package not found: $package"
    signature_result="$(/usr/sbin/pkgutil --check-signature "$package" 2>&1)" ||
        fail "Signature verification failed for $package: $signature_result"
    echo "Verified signed input: $package"
done

WORK_DIR="$(/usr/bin/mktemp -d "${TMPDIR:-/tmp}/eid-quickinstaller.XXXXXX")"
trap 'rm -rf "$WORK_DIR"' EXIT
for role in middleware esign esign-uninstall; do
    case "$role" in
        middleware) package="$MIDDLEWARE_PKG" ;;
        esign) package="$ESIGN_PKG" ;;
        esign-uninstall) package="$ESIGN_UNINSTALL_PKG" ;;
    esac
    /usr/sbin/pkgutil --expand "$package" "$WORK_DIR/$role"
done
xpath() { /usr/bin/xmllint --nonet --xpath "$1" "$2"; }

component_info() {
    local directory="$1" identifier="$2" info found="" count=0
    while IFS= read -r -d '' info; do
        if [[ "$(xpath 'string(/pkg-info/@identifier)' "$info")" == "$identifier" ]]; then
            found="$info"; count=$((count + 1))
        fi
    done < <(/usr/bin/find "$directory" -name PackageInfo -type f -print0)
    [[ "$count" -eq 1 ]] || fail "Expected one component: $identifier"
    [[ "$(xpath 'string(/pkg-info/@install-location)' "$found")" == / ]] ||
        fail "$identifier must install relative to /. Rebuild the system-wide package."
    echo "$found"
}
component_version() {
    local version
    version="$(xpath 'string(/pkg-info/@version)' "$1")"
    [[ "$version" =~ ^[0-9]+(\.[0-9]+)*$ ]] || fail "Invalid component version"
    echo "$version"
}
check_system_domain() {
    local distribution="$1" label="$2" system home anywhere
    [[ -f "$distribution" ]] || fail "$label must be a product package with a Distribution"
    system="$(xpath 'string(/installer-gui-script/domains/@enable_localSystem)' "$distribution")"
    home="$(xpath 'string(/installer-gui-script/domains/@enable_currentUserHome)' "$distribution")"
    anywhere="$(xpath 'string(/installer-gui-script/domains/@enable_anywhere)' "$distribution")"
    [[ -z "$system" || "$system" == true ]] || fail "$label must allow system installation"
    [[ -z "$home" || "$home" == false ]] || fail "$label must not allow home installation"
    [[ "$anywhere" == false || "$(xpath 'count(/installer-gui-script/options[@rootVolumeOnly="true"])' "$distribution")" -gt 0 ]] ||
        fail "$label must target only the system volume"
}
for role in middleware esign esign-uninstall; do
    check_system_domain "$WORK_DIR/$role/Distribution" "$role"
done
MW_INFO="$(component_info "$WORK_DIR/middleware" be.eid.middleware)"
TOKEN_INFO="$(component_info "$WORK_DIR/middleware" be.eid.BEIDtoken.app)"
ESIGN_INFO="$(component_info "$WORK_DIR/esign" com.zetes.bosa-pc-signing-macos)"
UNINSTALL_INFO="$(component_info "$WORK_DIR/esign-uninstall" com.zetes.bosa-pc-signing-macos)"
MIDDLEWARE_VERSION="$(component_version "$MW_INFO")"
ESIGN_VERSION="$(component_version "$ESIGN_INFO")"
[[ "$MIDDLEWARE_VERSION" == "$(component_version "$TOKEN_INFO")" ]] || fail "Middleware and BEIDToken package versions differ"
[[ "$ESIGN_VERSION" == "$(component_version "$UNINSTALL_INFO")" ]] || fail "esign installer and uninstaller versions differ"
[[ "$(xpath 'count(/pkg-info/payload)' "$ESIGN_INFO")" -eq 1 ]] || fail "esign installer has no application payload"
[[ "$(xpath 'count(/pkg-info/payload)' "$UNINSTALL_INFO")" -eq 0 &&
   "$(xpath 'count(/pkg-info/scripts/postinstall)' "$UNINSTALL_INFO")" -gt 0 ]] || fail "Expected a script-only esign uninstaller"

# Raw expansion keeps each original payload, script archive and receipt metadata.
# The signature on the product archive protects all embedded native components.
COMPONENTS="$WORK_DIR/components"
mkdir "$COMPONENTS"
/usr/sbin/pkgutil --flatten "$(dirname "$MW_INFO")" "$COMPONENTS/beidbuild.pkg"
/usr/sbin/pkgutil --flatten "$(dirname "$TOKEN_INFO")" "$COMPONENTS/BEIDToken.pkg"
/usr/sbin/pkgutil --flatten "$(dirname "$ESIGN_INFO")" "$COMPONENTS/BeIDSignComponent.pkg"
/usr/sbin/pkgutil --expand-full "$ESIGN_PKG" "$WORK_DIR/esign-payload"
APP_INFO=""
while IFS= read -r -d '' info; do
    [[ -z "$APP_INFO" ]] || fail "Multiple esign app bundles"
    APP_INFO="$info"
done < <(/usr/bin/find "$WORK_DIR/esign-payload" -path '*/Payload/Applications/esign.app/Contents/Info.plist' -type f -print0)
[[ -n "$APP_INFO" ]] || fail "esign app bundle is missing"
MIN_OS="$(/usr/libexec/PlistBuddy -c 'Print :LSMinimumSystemVersion' "$APP_INFO")"
[[ "$MIN_OS" =~ ^[0-9]+(\.[0-9]+)*$ ]] || fail "Invalid esign minimum macOS version"
/usr/bin/codesign --verify --deep --strict "$(dirname "$(dirname "$APP_INFO")")"

OUTPUT="${OUTPUT:-$SCRIPT_DIR/eID-Quickinstaller-$MIDDLEWARE_VERSION.dmg}"
PKG_OUTPUT="${PKG_OUTPUT:-${OUTPUT%.dmg}.pkg}"
[[ "$OUTPUT" == *.dmg && "$PKG_OUTPUT" == *.pkg ]] || fail "Outputs must have .dmg and .pkg extensions"
for destination in "$OUTPUT" "$PKG_OUTPUT"; do
    [[ ! -e "$destination" ]] || fail "Output already exists: $destination"
    [[ -d "$(dirname "$destination")" ]] || fail "Output directory is missing: $(dirname "$destination")"
done
OUTPUT="$(cd "$(dirname "$OUTPUT")" && pwd)/$(basename "$OUTPUT")"
PKG_OUTPUT="$(cd "$(dirname "$PKG_OUTPUT")" && pwd)/$(basename "$PKG_OUTPUT")"

RESOURCES="$WORK_DIR/resources"
/usr/bin/ditto "$WORK_DIR/middleware/Resources" "$RESOURCES"
ESIGN_LICENSE="$WORK_DIR/esign/Resources/LICENSE.html"
[[ -f "$ESIGN_LICENSE" ]] || fail "The esign product must include LICENSE.html; rebuild its packages"
/usr/bin/textutil -convert txt -stdout "$ESIGN_LICENSE" > "$WORK_DIR/esign-license.txt"
for language in en de fr nl; do
    license="$RESOURCES/$language.lproj/license.rtf"
    [[ -f "$license" ]] || fail "Middleware license is missing: $language"
    {
        printf '<html><meta charset="utf-8"><body><h1>eID middleware</h1><pre style="white-space:pre-wrap">'
        /usr/bin/textutil -convert txt -stdout "$license" | /usr/bin/sed 's/\&/\&amp;/g; s/</\&lt;/g; s/>/\&gt;/g'
        printf '</pre><h1>esign</h1><pre style="white-space:pre-wrap">'
        /usr/bin/sed 's/\&/\&amp;/g; s/</\&lt;/g; s/>/\&gt;/g' "$WORK_DIR/esign-license.txt"
        printf '</pre></body></html>\n'
    } > "$RESOURCES/$language.lproj/CombinedLicense.html"
done
/usr/bin/sed "s/@MIN_OS@/$MIN_OS/g" "$REPO_ROOT/installers/eid-mw/mac/Distribution_combined.xml" > "$WORK_DIR/Distribution.xml"
cat > "$RESOURCES/CombinedReadMe.html" <<README
<html><body><h1>Belgian eID middleware and esign</h1>
<p>This installer installs eID middleware $MIDDLEWARE_VERSION, BEIDToken and esign $ESIGN_VERSION for all users of this Mac. Administrator authorization is required.</p>
<p>After installation, esign opens automatically for the active desktop user when their graphical session is available. If it does not open, launch it from Applications. Other users launch esign from Applications in their own sessions. Each user completes their own setup and enables its background service when requested.</p>
<p>Adobe signing preferences and timestamp settings must be configured separately.</p>
<p>Installation and uninstallation preserve esign account data, preferences, caches, logs and protected containers in local user accounts, along with Adobe user preferences and signed documents. These packages do not provide a user-data reset; resetting protected data requires separate handling in esign's own user session.</p>
<p>The esign uninstaller removes only the shared esign application and its Acrobat plug-in. The eID middleware remains installed.</p>
<p>Requires macOS $MIN_OS or later.</p></body></html>
README

STAGING="$WORK_DIR/image"
mkdir "$STAGING"
product_args=(--distribution "$WORK_DIR/Distribution.xml" --resources "$RESOURCES" --package-path "$COMPONENTS")
if [[ "$SIGN" -eq 1 ]]; then
    product_args+=(--sign "$INSTALLER_IDENTITY" --timestamp)
fi
/usr/bin/productbuild "${product_args[@]}" "$STAGING/eID-Quickinstaller.pkg"
if [[ "$SIGN" -eq 1 ]]; then
    /usr/sbin/pkgutil --check-signature "$STAGING/eID-Quickinstaller.pkg"
fi
/usr/bin/ditto "$ESIGN_UNINSTALL_PKG" "$STAGING/esign-uninstall.pkg"
/usr/bin/cmp "$ESIGN_UNINSTALL_PKG" "$STAGING/esign-uninstall.pkg"
cat > "$STAGING/README.txt" <<README
Belgian eID Quickinstaller

Open eID-Quickinstaller.pkg to install eID middleware $MIDDLEWARE_VERSION,
BEIDToken and esign $ESIGN_VERSION in one installation for all users of this Mac.
Administrator authorization is required. esign is installed in /Applications.

After installation, esign opens automatically for the active desktop user when
their graphical session is available. If it does not open, launch it from
Applications. Other users launch esign from Applications in their own sessions.
Each user completes their own setup and enables its background service when
requested. Adobe signing preferences and timestamp settings must be configured
separately.

Installation and uninstallation preserve esign account data, preferences,
caches, logs and protected containers in local user accounts, along with Adobe
user preferences and signed documents. These packages do not provide a user-data
reset; resetting protected data requires separate handling in esign's own user
session.

When upgrading from a per-user esign installation, retire the old
~/Applications/esign.app copy and check background-service registration in that
user's session. Keeping both copies can select the wrong app when signing.

esign-uninstall.pkg removes only the shared esign application and its Acrobat plug-in.
It requires administrator authorization and leaves the eID middleware installed.

Requires macOS $MIN_OS or later.
README
/usr/bin/hdiutil create -fs HFS+ -format UDZO -volname 'eID Quickinstaller' \
    -srcfolder "$STAGING" "$WORK_DIR/combined.dmg"
if [[ "$SIGN" -eq 1 ]]; then
    /usr/bin/codesign --timestamp --sign "$SIGN_IDENTITY" "$WORK_DIR/combined.dmg"
    /usr/bin/codesign --verify --strict "$WORK_DIR/combined.dmg"
fi

if [[ "$NOTARIZE" -eq 1 ]]; then
    /usr/bin/xcrun notarytool submit "$WORK_DIR/combined.dmg" --wait --keychain-profile "$NOTARY_PROFILE" \
        --output-format json > "$WORK_DIR/notarization.json"
    status="$(/usr/bin/plutil -extract status raw -o - "$WORK_DIR/notarization.json")"
    [[ "$status" == Accepted ]] || { cat "$WORK_DIR/notarization.json" >&2; fail "Notarization status: $status"; }
    # Staple the sibling PKG after the outer submission; do not mutate the PKG inside the DMG.
    /usr/bin/xcrun stapler staple "$STAGING/eID-Quickinstaller.pkg"
    /usr/bin/xcrun stapler validate "$STAGING/eID-Quickinstaller.pkg"
    /usr/bin/xcrun stapler staple "$WORK_DIR/combined.dmg"
    /usr/bin/xcrun stapler validate "$WORK_DIR/combined.dmg"
fi
/usr/bin/ditto "$STAGING/eID-Quickinstaller.pkg" "$WORK_DIR/final.pkg"
mv -n "$WORK_DIR/final.pkg" "$PKG_OUTPUT"
[[ ! -e "$WORK_DIR/final.pkg" ]] || fail "Output already exists: $PKG_OUTPUT"
mv -n "$WORK_DIR/combined.dmg" "$OUTPUT"
[[ ! -e "$WORK_DIR/combined.dmg" ]] || fail "Output already exists: $OUTPUT"
echo "Created: $PKG_OUTPUT"
echo "Created: $OUTPUT (middleware $MIDDLEWARE_VERSION; esign $ESIGN_VERSION)"
