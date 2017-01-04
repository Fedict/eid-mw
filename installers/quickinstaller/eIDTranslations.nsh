LoadLanguageFile "${NSISDIR}\Contrib\Language files\English.nlf"
LoadLanguageFile "${NSISDIR}\Contrib\Language files\Dutch.nlf"
LoadLanguageFile "${NSISDIR}\Contrib\Language files\French.nlf"
LoadLanguageFile "${NSISDIR}\Contrib\Language files\German.nlf"



VIAddVersionKey /LANG=${LANG_ENGLISH} "ProductName" "Belgium eID Middleware"
VIAddVersionKey /LANG=${LANG_ENGLISH} "Comments" "Software for reading the Belgium Identity Card"
VIAddVersionKey /LANG=${LANG_ENGLISH} "CompanyName" "Fedict"
VIAddVersionKey /LANG=${LANG_ENGLISH} "LegalTrademarks" ""
VIAddVersionKey /LANG=${LANG_ENGLISH} "LegalCopyright" "Copyright � 2015"
VIAddVersionKey /LANG=${LANG_ENGLISH} "FileDescription" "eID QuickInstaller"


;below are the screen header texts, they should be 38 characters maximum
LangString ls_welcome ${LANG_ENGLISH} "Welcome to the eID QuickInstaller"
LangString ls_welcome ${LANG_FRENCH} "Bienvenue dans l'eID QuickInstaller"
LangString ls_welcome ${LANG_DUTCH} "Welkom bij de eID QuickInstaller"
LangString ls_welcome ${LANG_GERMAN} "Willkommen beim eID Installer"

LangString ls_complete ${LANG_ENGLISH} "Installation complete"
LangString ls_complete ${LANG_FRENCH} "Installation compl�te"
LangString ls_complete ${LANG_DUTCH} "Installatie voltooid"
LangString ls_complete ${LANG_GERMAN} "Installation abgeschlossen"

LangString ls_pleaseconnect ${LANG_ENGLISH} "Please connect your card reader"
LangString ls_pleaseconnect ${LANG_FRENCH} "Connectez votre lecteur de cartes"
LangString ls_pleaseconnect ${LANG_DUTCH} "Gelieve uw kaartlezer aan te sluiten"
LangString ls_pleaseconnect ${LANG_GERMAN} "Kartenleser bitte anschlie�en"

LangString ls_noreaderfound ${LANG_ENGLISH} "No card reader was found"
LangString ls_noreaderfound ${LANG_FRENCH} "Aucun lecteur de cartes d�tect�"
LangString ls_noreaderfound ${LANG_DUTCH} "Geen kaartlezer gevonden"
LangString ls_noreaderfound ${LANG_GERMAN} "Kein Kartenleser gefunden"

LangString ls_pleaseinsertcard ${LANG_ENGLISH} "Please insert your eID card"
LangString ls_pleaseinsertcard ${LANG_FRENCH} "Veuillez ins�rer votre carte eID"
LangString ls_pleaseinsertcard ${LANG_DUTCH} "Gelieve uw eID kaart in te steken"
LangString ls_pleaseinsertcard ${LANG_GERMAN} "Bitte stecken Sie Ihre eID-Karte ein"

LangString ls_nocardfound ${LANG_ENGLISH} "No eID card was found"
LangString ls_nocardfound ${LANG_FRENCH} "Aucune carte eID d�tect�e"
LangString ls_nocardfound ${LANG_DUTCH} "Geen eID kaart gevonden"
LangString ls_nocardfound ${LANG_GERMAN} "Keine eID-Karte gefunden"

LangString ls_cardread ${LANG_ENGLISH} "Card read"
LangString ls_cardread ${LANG_FRENCH} "Carte lue"
LangString ls_cardread ${LANG_DUTCH} "Kaart gelezen"
LangString ls_cardread ${LANG_GERMAN} "Karte gelesen"

;below are the content prefixes, preferably below 12 characters
LangString ls_name ${LANG_ENGLISH} "Name:"
LangString ls_name ${LANG_FRENCH} "Nom:"
LangString ls_name ${LANG_DUTCH} "Naam:"
LangString ls_name ${LANG_GERMAN} "Name:"

LangString ls_address ${LANG_ENGLISH} "Address:"
LangString ls_address ${LANG_FRENCH} "Adresse:"
LangString ls_address ${LANG_DUTCH} "Adres:"
LangString ls_address ${LANG_GERMAN} "Adresse:"

LangString ls_error ${LANG_ENGLISH} "Error:"
LangString ls_error ${LANG_FRENCH} "Erreur:"
LangString ls_error ${LANG_DUTCH} "Fout:"
LangString ls_error ${LANG_GERMAN} "Fehler:"

;below are the button's texts, they should be 12 characters maximum
LangString ls_install ${LANG_ENGLISH} "Install"
LangString ls_install ${LANG_FRENCH} "Installer"
LangString ls_install ${LANG_DUTCH} "Installeren"
LangString ls_install ${LANG_GERMAN} "Installieren"

LangString ls_cancel ${LANG_ENGLISH} "Cancel"
LangString ls_cancel ${LANG_FRENCH} "Annuler"
LangString ls_cancel ${LANG_DUTCH} "Annuleren"
LangString ls_cancel ${LANG_GERMAN} "Abbrechen"

LangString ls_next ${LANG_ENGLISH} "Next"
LangString ls_next ${LANG_FRENCH} "Suivant"
LangString ls_next ${LANG_DUTCH} "Volgende"
LangString ls_next ${LANG_GERMAN} "Weiter"

LangString ls_back ${LANG_ENGLISH} "Back"
LangString ls_back ${LANG_FRENCH} "Pr�c�dent"
LangString ls_back ${LANG_DUTCH} "Vorige"
LangString ls_back ${LANG_GERMAN} "Zur�ck"

LangString ls_close ${LANG_ENGLISH} "Close"
LangString ls_close ${LANG_FRENCH} "Fermer"
LangString ls_close ${LANG_DUTCH} "Sluiten"
LangString ls_close ${LANG_GERMAN} "Beenden"

LangString ls_finish ${LANG_ENGLISH} "Finish"
LangString ls_finish ${LANG_FRENCH} "Terminer"
LangString ls_finish ${LANG_DUTCH} "Afsluiten"
LangString ls_finish ${LANG_GERMAN} "Beenden"

LangString ls_test ${LANG_ENGLISH} "Test"
LangString ls_test ${LANG_FRENCH} "Tester"
LangString ls_test ${LANG_DUTCH} "Testen"
LangString ls_test ${LANG_GERMAN} "Pr�fen"

;below texts are error reports, they are show in a message box, no length limits
LangString ls_testfailed ${LANG_ENGLISH} "STR:Test failed,$\n$\ngo back and try again,$\nor reboot your pc and launch the eID Viewer to continue testing"
LangString ls_testfailed ${LANG_FRENCH} "STR:�chec du test,$\n$\nretour � l��cran pr�c�dent pour r�essayer,$\nou red�marrez votre PC et lancez l�eID Viewer pour continuer le test"
LangString ls_testfailed ${LANG_DUTCH} "STR:Test gefaald,$\n$\nga terug en probeer opnieuw,$\nof herstart uw pc en start de eID Viewer om verder te testen"
LangString ls_testfailed ${LANG_GERMAN} "STR:Test fehlgeschlagen,$\n$\nzur�ck zum vorherigen Bildschirm und erneut versuchen,$\noder den PC neu starten und eID Viewer starten, um den Test fortzusetzen"

LangString ls_errorreadersearch ${LANG_ENGLISH} "Error while searching for card readers"
LangString ls_errorreadersearch ${LANG_FRENCH} "Erreur d�tect�e pendant la recherche de lecteurs de cartes"
LangString ls_errorreadersearch ${LANG_DUTCH} "Probleem opgetreden bij het zoeken naar kaartlezers"
LangString ls_errorreadersearch ${LANG_GERMAN} "Fehler bei der Suche nach Kartenlesern"

LangString ls_errorreadingcard ${LANG_ENGLISH} "Error while trying to read from card"
LangString ls_errorreadingcard ${LANG_FRENCH} "Erreur d�tect�e pendant la tentative de lecture de la carte"
LangString ls_errorreadingcard ${LANG_DUTCH} "Probleem opgetreden bij het lezen van de kaart"
LangString ls_errorreadingcard ${LANG_GERMAN} "Fehler beim Versuch, die Karte zu lesen"

LangString ls_errorinstallmsi ${LANG_ENGLISH} "An error occurred while trying to install the eID Middleware. $\nPlease check the log for further details:"
LangString ls_errorinstallmsi ${LANG_FRENCH} "Une erreur est survenue lors de l�installation du middleware de l�eID. $\nVeuillez consulter le fichier journal pour obtenir de plus amples d�tails :"
LangString ls_errorinstallmsi ${LANG_DUTCH} "Er is een probleem opgetreden bij het installeren van de eID Middleware. $\nGelieve het logbestand te bekijken voor meer details:"
LangString ls_errorinstallmsi ${LANG_GERMAN} "Bei der Installation der eID Middleware ist ein Problem aufgetreten. $\nBitte pr�fen Sie die Log-Datei f�r weitere Einzelheiten:"

LangString ls_errorcopyfile ${LANG_ENGLISH} "An error occurred while trying to create file"
LangString ls_errorcopyfile ${LANG_FRENCH} "Une erreur est survenue lors de la cr�ation du fichier"
LangString ls_errorcopyfile ${LANG_DUTCH} "Er is een probleem opgetreden bij het aanmaken van bestand"
LangString ls_errorcopyfile ${LANG_GERMAN} "Fehler bei der Erstellung von Datei"

;below are texts linked to the bitmaps, no length limits
LangString ls_bitmapwelcome ${LANG_ENGLISH} "Background welcome image"
LangString ls_bitmapwelcome ${LANG_FRENCH} "Image de bienvenue en arri�re-plan"
LangString ls_bitmapwelcome ${LANG_DUTCH} "Achtergrond welkomafbeelding"
LangString ls_bitmapwelcome ${LANG_GERMAN} "Hintergrund-Begr��ungsbild"

LangString ls_bitmapconnectreader ${LANG_ENGLISH} "Background image asking to connect reader"
LangString ls_bitmapconnectreader ${LANG_FRENCH} "Image en arri�re-plan demandant de connecter le lecteur"
LangString ls_bitmapconnectreader ${LANG_DUTCH} "Achtergrondafbeelding die vraagt om de lezer aan te sluiten"
LangString ls_bitmapconnectreader ${LANG_GERMAN} "Hintergrundbild mit der Aufforderung, den Leser anzuschlie�en"

LangString ls_bitmapinsertcard ${LANG_ENGLISH} "Background image asking to insert eID card into the card reader"
LangString ls_bitmapinsertcard ${LANG_FRENCH} "Image en arri�re-plan demandant d�ins�rer l�eID dans le lecteur de cartes"
LangString ls_bitmapinsertcard ${LANG_DUTCH} "Achtergrondafbeelding die vraagt om de eID kaart in de kaartlezer te steken"
LangString ls_bitmapinsertcard ${LANG_GERMAN} "Hintergrundbild mit der Aufforderung, die eID-Karte in den Kartenleser zu stecken"

;below are language dependant urls
LangString ls_firefoxeidpluginpage ${LANG_ENGLISH} "https://addons.mozilla.org/en/firefox/addon/belgium-eid/?src=search"
LangString ls_firefoxeidpluginpage ${LANG_FRENCH} "https://addons.mozilla.org/fr/firefox/addon/belgium-eid/?src=search"
LangString ls_firefoxeidpluginpage ${LANG_DUTCH} "https://addons.mozilla.org/nl/firefox/addon/belgium-eid/?src=search"
LangString ls_firefoxeidpluginpage ${LANG_GERMAN} "https://addons.mozilla.org/de/firefox/addon/belgium-eid/?src=search"