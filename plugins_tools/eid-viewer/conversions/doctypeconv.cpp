#include "doctypeconv.h"
#include "cppeidstring.h"
#include <eid-util/utftranslate.h>

#include <sstream>

std::map < eid_vwr_langs, std::map < int,
	EID_STRING > >DocTypeConvertor::conversions;

DocTypeConvertor::DocTypeConvertor()
{
	if (conversions.empty())
	{
		conversions[EID_VWR_LANG_DE][1] = TEXT("PERSONALAUSWEIS");
		conversions[EID_VWR_LANG_DE][6] = TEXT("KIDS-KARTE");
		conversions[EID_VWR_LANG_DE][7] = TEXT("STARTKARTE");
		conversions[EID_VWR_LANG_DE][8] = TEXT("Berechtigungskarte");
		// Foreigner A
		conversions[EID_VWR_LANG_DE][11] =
			TEXT
			("Bescheinigung der Eintragung im Ausl\u00e4nderregister;Vor\u00fcbergehender Aufenthalt");
		// Foreigner B
		conversions[EID_VWR_LANG_DE][12] =
			TEXT
			("Bescheinigung der Eintragung im Ausl\u00e4nderregister");
		// Foreigner C
		conversions[EID_VWR_LANG_DE][13] =
			TEXT("Personalausweis f\u00fcr Ausl\u00e4nder");
		// Foreigner D
		conversions[EID_VWR_LANG_DE][14] =
			TEXT("Daueraufenthalt \u2013 EU");
		// Foreigner E
		conversions[EID_VWR_LANG_DE][15] =
			TEXT
			("Anmeldebescheinigung");
		// Foreigner E+
		conversions[EID_VWR_LANG_DE][16] =
			TEXT
			("Dokument zur Bescheinigung des Daueraufenthalts");
		// Foreigner F
		conversions[EID_VWR_LANG_DE][17] =
			TEXT
			("Ausweis f\u00fcr Nicht-EU Familienmitglieder;eines EU-Ausl\u00e4nders;oder eines belgischen Staatsangeh\u00f6rigen;Aufenthaltskarte f\u00fcr Familienangeh\u00f6rige eines Unionsb\u00fcrgers");
		// Foreigner F+
		conversions[EID_VWR_LANG_DE][18] =
			TEXT
			("Daueraufenthaltskarte f\u00fcr Familienangeh\u00f6rige eines Unionsb\u00fcrgers");
		// European blue card H
		conversions[EID_VWR_LANG_DE][19] = TEXT("Blaue Karte EU");

		conversions[EID_VWR_LANG_EN][1] = TEXT("IDENTITY CARD");
		conversions[EID_VWR_LANG_EN][6] = TEXT("KIDS CARD");
		conversions[EID_VWR_LANG_EN][7] = TEXT("BOOTSTRAP CARD");
		conversions[EID_VWR_LANG_EN][8] = TEXT("Habilitation Card");
		// Foreigner A
		conversions[EID_VWR_LANG_EN][11] =
			TEXT
			("Proof of registration in the Aliens\u2019 Register \u2013 Temporary residence");
		// Foreigner B
		conversions[EID_VWR_LANG_EN][12] =
			TEXT
			("Proof of registration in the Aliens\u2019 Register");
		// Foreigner C
		conversions[EID_VWR_LANG_EN][13] =
			TEXT("Identity Card for Aliens");
		// Foreigner D
		conversions[EID_VWR_LANG_EN][14] =
			TEXT("Long-term EU Resident");
		// Foreigner E
		conversions[EID_VWR_LANG_EN][15] =
			TEXT
			("Registration certificate");
		// Foreigner E+
		conversions[EID_VWR_LANG_EN][16] =
			TEXT
			("Document certifying permanent residence");
		// Foreigner F
		conversions[EID_VWR_LANG_EN][17] =
			TEXT
			("Residence card of a family member of a Union citizen");
		// Foreigner F+
		conversions[EID_VWR_LANG_EN][18] =
			TEXT
			("Permanent residence card of a family member of a Union citizen");
		// European blue card H
		conversions[EID_VWR_LANG_EN][19] =
			TEXT("EU Blue Card");

		conversions[EID_VWR_LANG_FR][1] = TEXT("CARTE D'IDENTITE");
		conversions[EID_VWR_LANG_FR][6] = TEXT("CARTE POUR ENFANTS");
		conversions[EID_VWR_LANG_FR][7] = TEXT("CARTE DE DEMARRAGE");
		conversions[EID_VWR_LANG_FR][8] =
			TEXT("CARTE D'HABILITATION");
		// Foreigner A
		conversions[EID_VWR_LANG_FR][11] =
			TEXT
			("Certificat d'inscription au Registre des Etrangers \u2013;S\u00e9jour temporaire");
		// Foreigner B
		conversions[EID_VWR_LANG_FR][12] =
			TEXT
			("Certificat d'inscription au Registre des Etrangers");
		// Foreigner C
		conversions[EID_VWR_LANG_FR][13] =
			TEXT("Carte d'identit\u00e9 d'\u00e9tranger");
		// Foreigner D
		conversions[EID_VWR_LANG_FR][14] =
			TEXT("R\u00e9sident de longue dur\u00e9e \u2013 UE");
		// Foreigner E
		conversions[EID_VWR_LANG_FR][15] =
			TEXT
			("Attestation d'enregistrement");
		// Foreigner E+
		conversions[EID_VWR_LANG_FR][16] =
			TEXT
			("Document attestant de la permanence du s\u00e9jour");
		// Foreigner F
		conversions[EID_VWR_LANG_FR][17] =
			TEXT
			("Carte de s\u00e9jour de membre de la famille d'un citoyen de l'Union");
		// Foreigner F+
		conversions[EID_VWR_LANG_FR][18] =
			TEXT
			("Carte de s\u00e9jour permanent de membre de la famille d\u2019un citoyen de l\u2019Union");
		// European blue card H
		conversions[EID_VWR_LANG_FR][19] =
			TEXT("Carte bleue europ\u00e9enne");

		conversions[EID_VWR_LANG_NL][1] = TEXT("IDENTITEITSKAART");
		conversions[EID_VWR_LANG_NL][6] = TEXT("KIDS KAART");
		conversions[EID_VWR_LANG_NL][7] = TEXT("OPSTARTKAART");
		conversions[EID_VWR_LANG_NL][8] = TEXT("HABILITATIEKAART");
		// Foreigner A");
		conversions[EID_VWR_LANG_NL][11] =
			TEXT
			("Bewijs van inschrijving in het vreemdelingenregister - Tijdelijk verblijf");
		// Foreigner B");
		conversions[EID_VWR_LANG_NL][12] =
			TEXT
			("Bewijs van inschrijving in het vreemdelingenregister");
		// Foreigner C");
		conversions[EID_VWR_LANG_NL][13] =
			TEXT("Identiteitskaart voor vreemdeling");
		// Foreigner D");
		conversions[EID_VWR_LANG_NL][14] =
			TEXT("EU-Langdurig ingezetene");
		// Foreigner E");
		conversions[EID_VWR_LANG_NL][15] =
			TEXT
			("Verklaring van inschrijving");
		// Foreigner E+");
		conversions[EID_VWR_LANG_NL][16] =
			TEXT
			("Document ter staving van duurzaam verblijf van een EU onderdaan");
		// Foreigner F");
		conversions[EID_VWR_LANG_NL][17] =
			TEXT
			("Verblijfskaart van een familielid van een burger van de Unie");
		// Foreigner F+");
		conversions[EID_VWR_LANG_NL][18] =
			TEXT
			("Duurzame verblijfskaart van een familielid van een burger van de Unie");
		// European blue card H");
		conversions[EID_VWR_LANG_NL][19] =
			TEXT("Europese blauwe kaart");
	}
}

EID_STRING DocTypeConvertor::convert(const void *normal)
{
	if (conversions.count(target_) > 0)
	{
		std::stringstream stream((char *) normal);
		int i;

		stream >> i;
		if (conversions[target_].count(i) > 0)
		{
			return conversions[target_][i];
		}
	}
	return (EID_CHAR *) normal;
}
