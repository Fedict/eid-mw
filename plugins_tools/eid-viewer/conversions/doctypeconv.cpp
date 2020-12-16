#include "doctypeconv.h"
#include "cppeidstring.h"
#include <eid-util/utftranslate.h>

#include <sstream>

std::map < eid_vwr_langs, std::map < EID_STRING, EID_STRING > >DocTypeConvertor::conversions;

DocTypeConvertor::DocTypeConvertor()
{
	if (conversions.empty())
	{
		conversions[EID_VWR_LANG_DE][TEXT("1")] = TEXT("PERSONALAUSWEIS");//according to spec it should be 1
		conversions[EID_VWR_LANG_DE][TEXT("6")] = TEXT("KIDS-KARTE");
		conversions[EID_VWR_LANG_DE][TEXT("7")] = TEXT("STARTKARTE");
		conversions[EID_VWR_LANG_DE][TEXT("8")] = TEXT("Berechtigungskarte");
		conversions[EID_VWR_LANG_DE][TEXT("01")] = TEXT("PERSONALAUSWEIS");// in the field, both 1 and 01 are present
		conversions[EID_VWR_LANG_DE][TEXT("06")] = TEXT("KIDS-KARTE");
		conversions[EID_VWR_LANG_DE][TEXT("07")] = TEXT("STARTKARTE");
		conversions[EID_VWR_LANG_DE][TEXT("08")] = TEXT("Berechtigungskarte");
		// Foreigner A
		conversions[EID_VWR_LANG_DE][TEXT("11")] = TEXT("Bescheinigung der Eintragung im Ausl\u00e4nderregister \u2013 Vor\u00fcbergehender Aufenthalt");
		// Foreigner B
		conversions[EID_VWR_LANG_DE][TEXT("12")] = TEXT("Bescheinigung der Eintragung im Ausl\u00e4nderregister");
		// Foreigner C
		conversions[EID_VWR_LANG_DE][TEXT("13")] = TEXT("Personalausweis f\u00fcr Ausl\u00e4nder");
		// Foreigner D
		conversions[EID_VWR_LANG_DE][TEXT("14")] = TEXT("Daueraufenthalt \u2013 EU");
		// Foreigner E
		conversions[EID_VWR_LANG_DE][TEXT("15")] = TEXT("Anmeldebescheinigung");
		// Foreigner E+
		conversions[EID_VWR_LANG_DE][TEXT("16")] = TEXT("Dokument zur Bescheinigung des Daueraufenthalts");
		// Foreigner F
		conversions[EID_VWR_LANG_DE][TEXT("17")] = TEXT("Ausweis f\u00fcr Nicht-EU Familienmitglieder;eines EU-Ausl\u00e4nders;oder eines belgischen Staatsangeh\u00f6rigen;Aufenthaltskarte f\u00fcr Familienangeh\u00f6rige eines Unionsb\u00fcrgers");
		// Foreigner F+
		conversions[EID_VWR_LANG_DE][TEXT("18")] = TEXT("Daueraufenthaltskarte f\u00fcr Familienangeh\u00f6rige eines Unionsb\u00fcrgers");
		// European blue card H
		conversions[EID_VWR_LANG_DE][TEXT("19")] = TEXT("Blaue Karte EU");
		// card I");
		conversions[EID_VWR_LANG_DE][TEXT("20")] = TEXT("I. ICT");
		// card J");
		conversions[EID_VWR_LANG_DE][TEXT("21")] = TEXT("J. Mobile ICT");

		conversions[EID_VWR_LANG_EN][TEXT("1")] = TEXT("IDENTITY CARD");
		conversions[EID_VWR_LANG_EN][TEXT("6")] = TEXT("KIDS CARD");
		conversions[EID_VWR_LANG_EN][TEXT("7")] = TEXT("BOOTSTRAP CARD");
		conversions[EID_VWR_LANG_EN][TEXT("8")] = TEXT("Habilitation Card");
		conversions[EID_VWR_LANG_EN][TEXT("01")] = TEXT("IDENTITY CARD");
		conversions[EID_VWR_LANG_EN][TEXT("06")] = TEXT("KIDS CARD");
		conversions[EID_VWR_LANG_EN][TEXT("07")] = TEXT("BOOTSTRAP CARD");
		conversions[EID_VWR_LANG_EN][TEXT("08")] = TEXT("Habilitation Card");
		// Foreigner A
		conversions[EID_VWR_LANG_EN][TEXT("11")] = TEXT("Proof of registration in the Aliens\u2019 Register - Temporary residence");
		// Foreigner B
		conversions[EID_VWR_LANG_EN][TEXT("12")] = TEXT("Proof of registration in the Aliens\u2019 Register");
		// Foreigner C
		conversions[EID_VWR_LANG_EN][TEXT("13")] = TEXT("Identity Card for Aliens");
		// Foreigner D
		conversions[EID_VWR_LANG_EN][TEXT("14")] = TEXT("Long-term EU Resident");
		// Foreigner E
		conversions[EID_VWR_LANG_EN][TEXT("15")] = TEXT("Registration certificate");
		// Foreigner E+
		conversions[EID_VWR_LANG_EN][TEXT("16")] = TEXT("Document certifying permanent residence");
		// Foreigner F
		conversions[EID_VWR_LANG_EN][TEXT("17")] = TEXT("Residence card of a family member of a Union citizen");
		// Foreigner F+
		conversions[EID_VWR_LANG_EN][TEXT("18")] = TEXT("Permanent residence card of a family member of a Union citizen");
		// European blue card H
		conversions[EID_VWR_LANG_EN][TEXT("19")] = TEXT("EU Blue Card");
		// card I");
		conversions[EID_VWR_LANG_EN][TEXT("20")] = TEXT("I. ICT");
		// card J");
		conversions[EID_VWR_LANG_EN][TEXT("21")] = TEXT("J. Mobile ICT");

		conversions[EID_VWR_LANG_FR][TEXT("1")] = TEXT("CARTE D'IDENTITE");
		conversions[EID_VWR_LANG_FR][TEXT("6")] = TEXT("CARTE POUR ENFANTS");
		conversions[EID_VWR_LANG_FR][TEXT("7")] = TEXT("CARTE DE DEMARRAGE");
		conversions[EID_VWR_LANG_FR][TEXT("8")] = TEXT("CARTE D'HABILITATION");
		conversions[EID_VWR_LANG_FR][TEXT("01")] = TEXT("CARTE D'IDENTITE");
		conversions[EID_VWR_LANG_FR][TEXT("06")] = TEXT("CARTE POUR ENFANTS");
		conversions[EID_VWR_LANG_FR][TEXT("07")] = TEXT("CARTE DE DEMARRAGE");
		conversions[EID_VWR_LANG_FR][TEXT("08")] = TEXT("CARTE D'HABILITATION");
		// Foreigner A
		conversions[EID_VWR_LANG_FR][TEXT("11")] = TEXT("Certificat d'inscription au Registre des Etrangers \u2013 S\u00e9jour temporaire");
		// Foreigner B
		conversions[EID_VWR_LANG_FR][TEXT("12")] = TEXT("Certificat d'inscription au Registre des Etrangers");
		// Foreigner C
		conversions[EID_VWR_LANG_FR][TEXT("13")] = TEXT("Carte d'identit\u00e9 d'\u00e9tranger");
		// Foreigner D
		conversions[EID_VWR_LANG_FR][TEXT("14")] = TEXT("R\u00e9sident de longue dur\u00e9e \u2013 UE");
		// Foreigner E
		conversions[EID_VWR_LANG_FR][TEXT("15")] = TEXT("Attestation d'enregistrement");
		// Foreigner E+	
		conversions[EID_VWR_LANG_FR][TEXT("16")] = TEXT("Document attestant de la permanence du s\u00e9jour");
		// Foreigner F
		conversions[EID_VWR_LANG_FR][TEXT("17")] = TEXT("Carte de s\u00e9jour de membre de la famille d'un citoyen de l'Union");
		// Foreigner F+	
		conversions[EID_VWR_LANG_FR][TEXT("18")] = TEXT("Carte de s\u00e9jour permanent de membre de la famille d\u2019un citoyen de l\u2019Union");
		// European blue card H
		conversions[EID_VWR_LANG_FR][TEXT("19")] = TEXT("Carte bleue europ\u00e9enne");
		// card I");
		conversions[EID_VWR_LANG_FR][TEXT("20")] = TEXT("I. ICT");
		// card J");
		conversions[EID_VWR_LANG_FR][TEXT("21")] = TEXT("J. Mobile ICT");

		conversions[EID_VWR_LANG_NL][TEXT("1")] = TEXT("IDENTITEITSKAART");
		conversions[EID_VWR_LANG_NL][TEXT("6")] = TEXT("KIDS KAART");
		conversions[EID_VWR_LANG_NL][TEXT("7")] = TEXT("OPSTARTKAART");
		conversions[EID_VWR_LANG_NL][TEXT("8")] = TEXT("HABILITATIEKAART");
		conversions[EID_VWR_LANG_NL][TEXT("01")] = TEXT("IDENTITEITSKAART");
		conversions[EID_VWR_LANG_NL][TEXT("06")] = TEXT("KIDS KAART");
		conversions[EID_VWR_LANG_NL][TEXT("07")] = TEXT("OPSTARTKAART");
		conversions[EID_VWR_LANG_NL][TEXT("08")] = TEXT("HABILITATIEKAART");
		// Foreigner A");
		conversions[EID_VWR_LANG_NL][TEXT("11")] = TEXT("Bewijs van inschrijving in het vreemdelingenregister \u2013 Tijdelijk verblijf");
		// Foreigner B");
		conversions[EID_VWR_LANG_NL][TEXT("12")] = TEXT("Bewijs van inschrijving in het vreemdelingenregister");
		// Foreigner C");
		conversions[EID_VWR_LANG_NL][TEXT("13")] = TEXT("Identiteitskaart voor vreemdeling");
		// Foreigner D");
		conversions[EID_VWR_LANG_NL][TEXT("14")] = TEXT("EU-Langdurig ingezetene");
		// Foreigner E");
		conversions[EID_VWR_LANG_NL][TEXT("15")] = TEXT("Verklaring van inschrijving");
		// Foreigner E+");
		conversions[EID_VWR_LANG_NL][TEXT("16")] = TEXT("Document ter staving van duurzaam verblijf van een EU onderdaan");
		// Foreigner F");
		conversions[EID_VWR_LANG_NL][TEXT("17")] = TEXT("Verblijfskaart van een familielid van een burger van de Unie");
		// Foreigner F+");
		conversions[EID_VWR_LANG_NL][TEXT("18")] = TEXT("Duurzame verblijfskaart van een familielid van een burger van de Unie");
		// European blue card H");
		conversions[EID_VWR_LANG_NL][TEXT("19")] = TEXT("Europese blauwe kaart");
		// card I");
		conversions[EID_VWR_LANG_NL][TEXT("20")] = TEXT("I. ICT");
		// card J");
		conversions[EID_VWR_LANG_NL][TEXT("21")] = TEXT("J. Mobile ICT");
	}
}

EID_STRING DocTypeConvertor::convert(const void *original)
{
	if (conversions.count(target_) > 0)
	{
		const EID_CHAR *eid_original = (const EID_CHAR *)original;

		if (conversions[target_].count(eid_original) > 0)
		{
			return conversions[target_][eid_original];
		}
	}
	return (EID_CHAR *)original;
}
