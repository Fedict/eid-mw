LoadLanguageFile "${NSISDIR}\Contrib\Language files\English.nlf"
LoadLanguageFile "${NSISDIR}\Contrib\Language files\Dutch.nlf"
LoadLanguageFile "${NSISDIR}\Contrib\Language files\French.nlf"
LoadLanguageFile "${NSISDIR}\Contrib\Language files\German.nlf"



;below are the screen header texts, they should be 38 characters maximum
LangString ls_welcome_short ${LANG_ENGLISH} "Welcome!"
LangString ls_welcome_short ${LANG_FRENCH} "Bienvenue!"
LangString ls_welcome_short ${LANG_DUTCH} "Welkom!"
LangString ls_welcome_short ${LANG_GERMAN} "Willkommen!"

LangString ls_welcome ${LANG_ENGLISH} "Welcome to the eID QuickInstaller"
LangString ls_welcome ${LANG_FRENCH} "Bienvenue dans l'eID QuickInstaller"
LangString ls_welcome ${LANG_DUTCH} "Welkom bij de eID QuickInstaller"
LangString ls_welcome ${LANG_GERMAN} "Willkommen beim eID Installer"

LangString ls_welcome_info ${LANG_ENGLISH} "Click 'Install' to install the eID software"
LangString ls_welcome_info ${LANG_FRENCH} "Cliquez sur $\"Installer$\" pour installer le logiciel eID"
LangString ls_welcome_info ${LANG_DUTCH} "Klik op 'Installeren' om de eID software te installeren"
LangString ls_welcome_info ${LANG_GERMAN} "Klicken Sie auf „Installieren“ um die eID-Software zu installieren."

LangString ls_welcome_viewer_info ${LANG_ENGLISH} "Click 'Install' to install the eIDViewer"
LangString ls_welcome_viewer_info ${LANG_FRENCH} "Cliquez sur $\"Installer$\" pour installer l'eIDViewer"
LangString ls_welcome_viewer_info ${LANG_DUTCH} "Klik op 'Installeren' om de eIDViewer te installeren"
LangString ls_welcome_viewer_info ${LANG_GERMAN} "Klicken Sie auf „Installieren“ um die eIDViewer zu installieren."

LangString ls_complete ${LANG_ENGLISH} "eID software installed!"
LangString ls_complete ${LANG_FRENCH} "Logiciel eID installé !"
LangString ls_complete ${LANG_DUTCH} "eID software geïnstalleerd!"
LangString ls_complete ${LANG_GERMAN} "eID-Software installiert!"

LangString ls_viewer_complete ${LANG_ENGLISH} "eIDViewer installed!"
LangString ls_viewer_complete ${LANG_FRENCH} "eIDViewer installé !"
LangString ls_viewer_complete ${LANG_DUTCH} "eIDViewer geïnstalleerd!"
LangString ls_viewer_complete ${LANG_GERMAN} "eIDViewer installiert!"

LangString ls_complete_info ${LANG_ENGLISH} "You can test the eID software using a suitable eID card reader.$\nClick 'Test' and follow the instructions step by step"
LangString ls_complete_info ${LANG_FRENCH} "Vous pouvez tester le logiciel eID avec un lecteur de cartes eID approprié.$\nCliquez sur $\"Tester$\" et suivez les instructions étape par étape."
LangString ls_complete_info ${LANG_DUTCH} "U kan de eID software testen met behulp van een geschikte eID kaartlezer.$\nKlik op 'Testen' en volg de instructies stap voor stap"
LangString ls_complete_info ${LANG_GERMAN} "Sie können die eID-Software mit einem geeigneten eID-Kartenleser testen.$\nKlicken Sie auf „Testen“ und befolgen Sie die Anweisungen Schritt für Schritt"

LangString ls_testcomplete_pre ${LANG_ENGLISH} "Dear "
LangString ls_testcomplete_pre ${LANG_FRENCH} "Cher/Chère"
LangString ls_testcomplete_pre ${LANG_DUTCH} "Beste"
LangString ls_testcomplete_pre ${LANG_GERMAN} "Hallo"

LangString ls_testcomplete_post ${LANG_ENGLISH} ",$\nthe software test was successful"
LangString ls_testcomplete_post ${LANG_FRENCH} ",$\nle test du logiciel a réussi"
LangString ls_testcomplete_post ${LANG_DUTCH} ",$\nde software test is geslaagd"
LangString ls_testcomplete_post ${LANG_GERMAN} ",$\nder Software-Test wurde erfolgreich durchgeführt"

LangString ls_testcomplete_info ${LANG_ENGLISH} "It is advisable to reboot your computer"
LangString ls_testcomplete_info ${LANG_FRENCH} "Nous vous recommandons de redémarrer votre ordinateur."
LangString ls_testcomplete_info ${LANG_DUTCH} "Herstarten van uw computer is aanbevolen"
LangString ls_testcomplete_info ${LANG_GERMAN} "Neustart Ihres Computers wird empfohlen"

LangString ls_install_failed ${LANG_ENGLISH} "eID software not installed"
LangString ls_install_failed ${LANG_FRENCH} "Le logiciel eID n'a pas été installé."
LangString ls_install_failed ${LANG_DUTCH} "eID software niet geïnstalleerd"
LangString ls_install_failed ${LANG_GERMAN} "eID-Software nicht installiert"

LangString ls_install_viewer_failed ${LANG_ENGLISH} "eIDViewer not installed"
LangString ls_install_viewer_failed ${LANG_FRENCH} "L'eIDViewer n'a pas été installé."
LangString ls_install_viewer_failed ${LANG_DUTCH} "eIDViewer niet geïnstalleerd"
LangString ls_install_viewer_failed ${LANG_GERMAN} "eIDViewer nicht installiert"

LangString ls_install_failed_info ${LANG_ENGLISH} "We were unable to install the eID software.$\nClick 'Help website' for further details"
LangString ls_install_failed_info ${LANG_FRENCH} "Nous n'avons pas pu installer le logiciel eID.$\nCliquez sur $\"Site web d'aide$\" pour plus d'informations."
LangString ls_install_failed_info ${LANG_DUTCH} "We hebben de eID software niet kunnen installeren.$\nKlik op 'Helpwebsite' voor meer informatie"
LangString ls_install_failed_info ${LANG_GERMAN} "Wir konnten die eID-Software nicht installieren.$\nKlicken Sie auf „Hilfe-Website“ für weitere Informationen"

LangString ls_install_viewer_failed_info ${LANG_ENGLISH} "We were unable to install the eIDViewer.$\nClick 'Help website' for further details"
LangString ls_install_viewer_failed_info ${LANG_FRENCH} "Nous n'avons pas pu installer l'eIDViewer.$\nCliquez sur $\"Site web d'aide$\" pour plus d'informations."
LangString ls_install_viewer_failed_info ${LANG_DUTCH} "We hebben de eIDViewer niet kunnen installeren.$\nKlik op 'Helpwebsite' voor meer informatie"
LangString ls_install_viewer_failed_info ${LANG_GERMAN} "Wir konnten die eIDViewer nicht installieren.$\nKlicken Sie auf „Hilfe-Website“ für weitere Informationen"

LangString ls_cardreader_failed ${LANG_ENGLISH} "No card reader found"
LangString ls_cardreader_failed ${LANG_FRENCH} "Lecteur de cartes non trouvé"
LangString ls_cardreader_failed ${LANG_DUTCH} "Kaartlezer niet gevonden"
LangString ls_cardreader_failed ${LANG_GERMAN} "Kartenleser nicht gefunden"

LangString ls_cardreader_failed_info ${LANG_ENGLISH} "Make sure it is properly connected. Then click 'Again'.$\nIf this problem persists, click 'Go to help website' for further details"
LangString ls_cardreader_failed_info ${LANG_FRENCH} "Vérifiez si le lecteur est branché correctement et cliquez sur $\"Recommencer$\".$\nSi le problème persiste, cliquez sur $\"Allez sur le site web d'aide$\" pour plus d'informations."
LangString ls_cardreader_failed_info ${LANG_DUTCH} "Controleer of deze correct is aangesloten en klik op 'Opnieuw'.$\nAls dit probleem aanhoudt, klik op 'Ga naar helpwebsite' voor meer informatie"
LangString ls_cardreader_failed_info ${LANG_GERMAN} "Kontrollieren Sie, ob dieser korrekt angeschlossen ist und klicken Sie auf „Wiederholen“.$\nWenn das Problem weiter besteht, klicken Sie auf „Zur Hilfe-Website“ für weitere Informationen"

LangString ls_test_failed ${LANG_ENGLISH} "The test was unsuccessful"
LangString ls_test_failed ${LANG_FRENCH} "Le test a échoué."
LangString ls_test_failed ${LANG_DUTCH} "De test is niet geslaagd"
LangString ls_test_failed ${LANG_GERMAN} "Der Test ist fehlgeschlagen"

LangString ls_test_failed_info ${LANG_ENGLISH} "We were unable to test the eID software.$\nClick 'Help website' for further details"
LangString ls_test_failed_info ${LANG_FRENCH} "Nous n'avons pas pu tester le logiciel eID.$\nCliquez sur $\"Site web d'aide$\" pour plus d'informations."
LangString ls_test_failed_info ${LANG_DUTCH} "We hebben de eID software niet kunnen testen.$\nKlik op 'Helpwebsite' voor meer informatie"
LangString ls_test_failed_info ${LANG_GERMAN} "Wir konnten die eID-Software nicht testen.$\nKlicken Sie auf „Hilfe-Website“ für weitere Informationen"

LangString ls_pleaseconnect ${LANG_ENGLISH} "Please connect your card reader"
LangString ls_pleaseconnect ${LANG_FRENCH} "Connectez votre lecteur de cartes"
LangString ls_pleaseconnect ${LANG_DUTCH} "Gelieve uw kaartlezer aan te sluiten"
LangString ls_pleaseconnect ${LANG_GERMAN} "Kartenleser bitte anschließen"

LangString ls_noreaderfound ${LANG_ENGLISH} "No card reader was found"
LangString ls_noreaderfound ${LANG_FRENCH} "Aucun lecteur de cartes détecté"
LangString ls_noreaderfound ${LANG_DUTCH} "Geen kaartlezer gevonden"
LangString ls_noreaderfound ${LANG_GERMAN} "Kein Kartenleser gefunden"

LangString ls_pleaseinsertcard ${LANG_ENGLISH} "Insert your eID into the card reader"
LangString ls_pleaseinsertcard ${LANG_FRENCH} "Insérez votre eID dans le lecteur de cartes."
LangString ls_pleaseinsertcard ${LANG_DUTCH} "Steek uw eID in de kaartlezer"
LangString ls_pleaseinsertcard ${LANG_GERMAN} "Stecken Sie Ihre eID in den Kartenleser"

LangString ls_nocardfound ${LANG_ENGLISH} "No eID card was found"
LangString ls_nocardfound ${LANG_FRENCH} "Aucune carte eID détectée"
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
LangString ls_back ${LANG_FRENCH} "Précédent"
LangString ls_back ${LANG_DUTCH} "Vorige"
LangString ls_back ${LANG_GERMAN} "Zurück"

LangString ls_retry ${LANG_ENGLISH} "Retry"
LangString ls_retry ${LANG_FRENCH} "Opnieuw"
LangString ls_retry ${LANG_DUTCH} "Opnieuw"
LangString ls_retry ${LANG_GERMAN} "Opnieuw"

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
LangString ls_test ${LANG_GERMAN} "Prüfen"

LangString ls_helpsite ${LANG_ENGLISH} "Helpwebsite"
LangString ls_helpsite ${LANG_FRENCH} "Site web d'aide"
LangString ls_helpsite ${LANG_DUTCH} "Helpwebsite"
LangString ls_helpsite ${LANG_GERMAN} "Helpwebsite"

LangString ls_caption ${LANG_ENGLISH} "eID software installation"
LangString ls_caption ${LANG_FRENCH} "Installation Logiciel eID"
LangString ls_caption ${LANG_DUTCH} "eID software installatie"
LangString ls_caption ${LANG_GERMAN} "Installation der eID-Software"

LangString ls_caption_viewer ${LANG_ENGLISH} "eIDViewer installation"
LangString ls_caption_viewer ${LANG_FRENCH} "Installation eIDViewer"
LangString ls_caption_viewer ${LANG_DUTCH} "eIDViewer installatie"
LangString ls_caption_viewer ${LANG_GERMAN} "Installation der eIDViewer"

LangString ls_completedtext ${LANG_ENGLISH} "ready"
LangString ls_completedtext ${LANG_FRENCH} "terminé"
LangString ls_completedtext ${LANG_DUTCH} "klaar"
LangString ls_completedtext ${LANG_GERMAN} "fertig"

;below texts are error reports, they are show in a message box, no length limits
LangString ls_testfailed ${LANG_ENGLISH} "STR:Test failed,$\n$\ngo back and try again,$\nor reboot your pc and launch the eID Viewer to continue testing"
LangString ls_testfailed ${LANG_FRENCH} "STR:échec du test,$\n$\nretour à l’écran précédent pour réessayer,$\nou redémarrez votre PC et lancez l’eID Viewer pour continuer le test"
LangString ls_testfailed ${LANG_DUTCH} "STR:Test gefaald,$\n$\nga terug en probeer opnieuw,$\nof herstart uw pc en start de eID Viewer om verder te testen"
LangString ls_testfailed ${LANG_GERMAN} "STR:Test fehlgeschlagen,$\n$\nzurück zum vorherigen Bildschirm und erneut versuchen,$\noder den PC neu starten und eID Viewer starten, um den Test fortzusetzen"

LangString ls_errorreadersearch ${LANG_ENGLISH} "Error while searching for card readers"
LangString ls_errorreadersearch ${LANG_FRENCH} "Erreur détectée pendant la recherche de lecteurs de cartes"
LangString ls_errorreadersearch ${LANG_DUTCH} "Probleem opgetreden bij het zoeken naar kaartlezers"
LangString ls_errorreadersearch ${LANG_GERMAN} "Fehler bei der Suche nach Kartenlesern"

LangString ls_errorreadingcard ${LANG_ENGLISH} "Error while trying to read from card"
LangString ls_errorreadingcard ${LANG_FRENCH} "Erreur détectée pendant la tentative de lecture de la carte"
LangString ls_errorreadingcard ${LANG_DUTCH} "Probleem opgetreden bij het lezen van de kaart"
LangString ls_errorreadingcard ${LANG_GERMAN} "Fehler beim Versuch, die Karte zu lesen"

LangString ls_errorinstallmsi ${LANG_ENGLISH} "An error occurred while trying to install the eID Middleware. $\nPlease check the log for further details:"
LangString ls_errorinstallmsi ${LANG_FRENCH} "Une erreur est survenue lors de l’installation du middleware de l’eID. $\nVeuillez consulter le fichier journal pour obtenir de plus amples détails :"
LangString ls_errorinstallmsi ${LANG_DUTCH} "Er is een probleem opgetreden bij het installeren van de eID Middleware. $\nGelieve het logbestand te bekijken voor meer details:"
LangString ls_errorinstallmsi ${LANG_GERMAN} "Bei der Installation der eID Middleware ist ein Problem aufgetreten. $\nBitte prüfen Sie die Log-Datei für weitere Einzelheiten:"

LangString ls_errorcopyfile ${LANG_ENGLISH} "An error occurred while trying to create file"
LangString ls_errorcopyfile ${LANG_FRENCH} "Une erreur est survenue lors de la création du fichier"
LangString ls_errorcopyfile ${LANG_DUTCH} "Er is een probleem opgetreden bij het aanmaken van bestand"
LangString ls_errorcopyfile ${LANG_GERMAN} "Fehler bei der Erstellung von Datei"

LangString ls_errorinstallmsi_1612 ${LANG_ENGLISH} "The installation source for this product is not available. $\nVerify that the source exists and that you can access it."; $\nPlease check the FAQ for further details:  $\nhttps://eid.belgium.be/en/technical-documentation#7567"
LangString ls_errorinstallmsi_1612 ${LANG_FRENCH} "La source d'installation de ce produit n'est pas disponible. $\nVérifiez que la source existe et que vous pouvez y accéder."; $\nVeuillez consulter le FAQ pour obtenir de plus amples détails : $\nhttps://eid.belgium.be/fr/technical-documentation#7567"
LangString ls_errorinstallmsi_1612 ${LANG_DUTCH} "De installatiebron voor dit product is niet beschikbaar. $\nControleer of de bron bestaat en of u toegang hebt tot de bron."; $\nGelieve de FAQ te bekijken voor meer details: $\nhttps://eid.belgium.be/nl/technical-documentation#7567"
LangString ls_errorinstallmsi_1612 ${LANG_GERMAN} "Die Installationsquelle für dieses Produkt ist nicht verfügbar. $\nStellen Sie sicher, dass die Quelle vorhanden ist und Sie darauf zugreifen können."; $\nBitte prüfen Sie die Log-Datei für weitere Einzelheiten: $\nhttps://eid.belgium.be/de/technical-documentation#7567"

LangString ls_errorinstallmsi_1618 ${LANG_ENGLISH} "Another installation is already in progress. $\nComplete that installation before proceeding with this install."
LangString ls_errorinstallmsi_1618 ${LANG_FRENCH} "Une autre installation est déjà en cours. $\nComplétez cette installation avant de procéder à cette installation."
LangString ls_errorinstallmsi_1618 ${LANG_DUTCH} "Er is al een installatie aan de gang. $\nVoltooi die installatie voordat je verder gaat met deze installatie."
LangString ls_errorinstallmsi_1618 ${LANG_GERMAN} "Eine andere Installation ist bereits in Arbeit. $\nKomplettieren Sie diese Installation, bevor Sie mit dieser Installation fortfahren."

LangString ls_errorinstallmsi_1638 ${LANG_ENGLISH} "A higher version of the eID Middleware is already installed. $\nTo remove the existing version of this product,$\nuse Add/Remove Programs in Control Panel."
LangString ls_errorinstallmsi_1638 ${LANG_FRENCH} "Une version supérieure du middleware de l’eID est déjà installée. $\nPour supprimer la version existante de ce produit, $\nutilisez Ajout/Suppression de programmes dans le Panneau de configuration."
LangString ls_errorinstallmsi_1638 ${LANG_DUTCH} "Een hogere versie van de eID Middleware is al geïnstalleerd. $\nOm de bestaande versie van dit product te verwijderen, $\ngebruikt u Programma's toevoegen/verwijderen in het Configuratiescherm."
LangString ls_errorinstallmsi_1638 ${LANG_GERMAN} "Eine höhere Version der eID Middleware ist bereits installiert. $\nUm die vorhandene Version dieses Produkts zu entfernen, $\nverwenden Sie Programme hinzufügen/entfernen in der Systemsteuerung."

LangString ls_errorinstallmsi_viewer_1638 ${LANG_ENGLISH} "A higher version of the eID Viewer is already installed. $\nTo remove the existing version of this product,$\nuse Add/Remove Programs in Control Panel."
LangString ls_errorinstallmsi_viewer_1638 ${LANG_FRENCH} "Une version supérieure de l’eID Viewer est déjà installée. $\nPour supprimer la version existante de ce produit, $\nutilisez Ajout/Suppression de programmes dans le Panneau de configuration."
LangString ls_errorinstallmsi_viewer_1638 ${LANG_DUTCH} "Een hogere versie van de eID Viewer is al geïnstalleerd. $\nOm de bestaande versie van dit product te verwijderen, $\ngebruikt u Programma's toevoegen/verwijderen in het Configuratiescherm."
LangString ls_errorinstallmsi_viewer_1638 ${LANG_GERMAN} "Eine höhere Version der eID Viewer ist bereits installiert. $\nUm die vorhandene Version dieses Produkts zu entfernen, $\nverwenden Sie Programme hinzufügen/entfernen in der Systemsteuerung."

;LangString ls_errorinstallmsi_1612_FAQurl ${LANG_ENGLISH} "https://eid.belgium.be/en/technical-documentation#7567"
;LangString ls_errorinstallmsi_1612_FAQurl ${LANG_FRENCH} "https://eid.belgium.be/fr/technical-documentation#7565"
;LangString ls_errorinstallmsi_1612_FAQurl ${LANG_DUTCH} "https://eid.belgium.be/nl/technische-documentatie#7564"
;LangString ls_errorinstallmsi_1612_FAQurl ${LANG_GERMAN} "https://eid.belgium.be/de/technische-dokumentation#7566"

;Use the site's error codes instead of linking direct to the FAQ itemnr.
LangString ls_errorinstallmsi_1612_FAQurl ${LANG_ENGLISH} "https://eid.belgium.be/en/error-codes/1612"
LangString ls_errorinstallmsi_1612_FAQurl ${LANG_FRENCH} "https://eid.belgium.be/fr/error-codes/1612"
LangString ls_errorinstallmsi_1612_FAQurl ${LANG_DUTCH} "https://eid.belgium.be/nl/error-codes/1612"
LangString ls_errorinstallmsi_1612_FAQurl ${LANG_GERMAN} "https://eid.belgium.be/de/error-codes/1612"

;below are texts linked to the bitmaps, no length limits
LangString ls_bitmapwelcome ${LANG_ENGLISH} "Background welcome image"
LangString ls_bitmapwelcome ${LANG_FRENCH} "Image de bienvenue en arrière-plan"
LangString ls_bitmapwelcome ${LANG_DUTCH} "Achtergrond welkomafbeelding"
LangString ls_bitmapwelcome ${LANG_GERMAN} "Hintergrund-Begrüßungsbild"

LangString ls_bitmapconnectreader ${LANG_ENGLISH} "Background image asking to connect reader"
LangString ls_bitmapconnectreader ${LANG_FRENCH} "Image en arrière-plan demandant de connecter le lecteur"
LangString ls_bitmapconnectreader ${LANG_DUTCH} "Achtergrondafbeelding die vraagt om de lezer aan te sluiten"
LangString ls_bitmapconnectreader ${LANG_GERMAN} "Hintergrundbild mit der Aufforderung, den Leser anzuschließen"

LangString ls_bitmapinsertcard ${LANG_ENGLISH} "Background image asking to insert eID card into the card reader"
LangString ls_bitmapinsertcard ${LANG_FRENCH} "Image en arrière-plan demandant d’insérer l’eID dans le lecteur de cartes"
LangString ls_bitmapinsertcard ${LANG_DUTCH} "Achtergrondafbeelding die vraagt om de eID kaart in de kaartlezer te steken"
LangString ls_bitmapinsertcard ${LANG_GERMAN} "Hintergrundbild mit der Aufforderung, die eID-Karte in den Kartenleser zu stecken"

;below are language dependant urls
LangString ls_firefoxeidpluginpage ${LANG_ENGLISH} "https://addons.mozilla.org/en/firefox/addon/belgium-eid/?src=search"
LangString ls_firefoxeidpluginpage ${LANG_FRENCH} "https://addons.mozilla.org/fr/firefox/addon/belgium-eid/?src=search"
LangString ls_firefoxeidpluginpage ${LANG_DUTCH} "https://addons.mozilla.org/nl/firefox/addon/belgium-eid/?src=search"
LangString ls_firefoxeidpluginpage ${LANG_GERMAN} "https://addons.mozilla.org/de/firefox/addon/belgium-eid/?src=search"