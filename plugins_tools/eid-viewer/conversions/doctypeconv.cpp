#include "doctypeconv.h"

#include <sstream>

std::map<eid_vwr_langs, std::map<int, std::string> > DocTypeConvertor::conversions;

DocTypeConvertor::DocTypeConvertor() {
	if(conversions.empty()) {
		conversions[EID_VWR_LANG_DE][1] = "PERSONALAUSWEIS";
		conversions[EID_VWR_LANG_DE][6] = "KIDS-KARTE";
		conversions[EID_VWR_LANG_DE][7] = "STARTKARTE";
		conversions[EID_VWR_LANG_DE][8] = "Berechtigungskarte";
		// Foreigner A
		conversions[EID_VWR_LANG_DE][11] = "Bescheinigung der Eintragung im Ausl\u00e4nderregister;Vor\u00fcbergehender Aufenthalt";
		// Foreigner B
		conversions[EID_VWR_LANG_DE][12] = "Bescheinigung der Eintragung im Ausl\u00e4nderregister";
		// Foreigner C
		conversions[EID_VWR_LANG_DE][13] = "Personalausweis f\u00fcr Ausl\u00e4nder";
		// Foreigner D
		conversions[EID_VWR_LANG_DE][14] = "Daueraufenthalt \u2013 EG";
		// Foreigner E
		conversions[EID_VWR_LANG_DE][15] = "Aufenthaltserlaubnis f\u00fcr Staatsangeh\u00f6rige;eines EWG-Mitgliedstaates;Anmeldebescheinigung";
		// Foreigner E+
		conversions[EID_VWR_LANG_DE][16] = "Dokument zur Bescheinigung des Daueraufenthalts;eines EU-Ausl\u00e4nders";
		// Foreigner F
		conversions[EID_VWR_LANG_DE][17] = "Ausweis f\u00fcr Nicht-EU Familienmitglieder;eines EU-Ausl\u00e4nders;oder eines belgischen Staatsangeh\u00f6rigen;Aufenthaltskarte f\u00fcr Familienangeh\u00f6rige eines Unionsb\u00fcrgers";
		// Foreigner F+
		conversions[EID_VWR_LANG_DE][18] = "Daueraufenthaltskarte f\u00fcr Familienangeh\u00f6rige eines Unionsb\u00fcrgers";
		// European blue card H
		conversions[EID_VWR_LANG_DE][19] = "H. Blaue Karte EU";

		conversions[EID_VWR_LANG_EN][1] = "IDENTITY CARD";
		conversions[EID_VWR_LANG_EN][6] = "KIDS CARD";
		conversions[EID_VWR_LANG_EN][7] = "BOOTSTRAP CARD";
		conversions[EID_VWR_LANG_EN][8] = "Habilitation Card";
		// Foreigner A
		conversions[EID_VWR_LANG_EN][11] = "Proof of registration in the Aliens\u2019 Register \u2013 Temporary residence";
		// Foreigner B
		conversions[EID_VWR_LANG_EN][12] = "Proof of registration in the Aliens\u2019 Register";
		// Foreigner C
		conversions[EID_VWR_LANG_EN][13] = "Identity Card for Aliens";
		// Foreigner D
		conversions[EID_VWR_LANG_EN][14] = "Long-term EU Resident";
		// Foreigner E
		conversions[EID_VWR_LANG_EN][15] = "(Residence) permit of EU Citizen \u2013 Certificate of Registration";
		// Foreigner E+
		conversions[EID_VWR_LANG_EN][16] = "Document in evidence of durable residence of an EU Citizen";
		// Foreigner F
		conversions[EID_VWR_LANG_EN][17] = "Permit for non-EU family members of an EU Citizen or Belgian national";
		// Foreigner F+
		conversions[EID_VWR_LANG_EN][18] = "Durable residence permit of a family member of an EU Citizen";
		// European blue card H
		conversions[EID_VWR_LANG_EN][19] = "H. European Blue Card";

		conversions[EID_VWR_LANG_FR][1] = "CARTE D'IDENTITE";
		conversions[EID_VWR_LANG_FR][6] = "CARTE POUR ENFANTS";
		conversions[EID_VWR_LANG_FR][7] = "CARTE DE DEMARRAGE";
		conversions[EID_VWR_LANG_FR][8] = "CARTE D'HABILITATION";
		// Foreigner A
		conversions[EID_VWR_LANG_FR][11] = "Certificat d'inscription au Registre des Etrangers \u2013;S\u00e9jour temporaire";
		// Foreigner B
		conversions[EID_VWR_LANG_FR][12] = "Certificat d'inscription au Registre des Etrangers";
		// Foreigner C
		conversions[EID_VWR_LANG_FR][13] = "Carte d'identit\u00e9 d'\u00e9tranger";
		// Foreigner D
		conversions[EID_VWR_LANG_FR][14] = "R\u00e9sident de longue dur\u00e9e \u2013 CE";
		// Foreigner E
		conversions[EID_VWR_LANG_FR][15] = "Carte (de s\u00e9jour) de ressortissant;d\u2019un Etat membre de la CEE;Attestation d\u2019enregistrement";
		// Foreigner E+
		conversions[EID_VWR_LANG_FR][16] = "Document attestant de la permanence du s\u00e9jour;d\u2019un ressortissant de l'UE";
		// Foreigner F
		conversions[EID_VWR_LANG_FR][17] = "Carte pour les \u00e9trangers non UE qui sont membres de famille;d\u2019un ressortissant UE ou d\u2019un Belge; Carte de s\u00e9jour de membre de la famille d\u2019un citoyen de l\u2019Union";
		// Foreigner F+
		conversions[EID_VWR_LANG_FR][18] = "Carte de s\u00e9jour permanent de membre de la famille;d\u2019un citoyen de l\u2019Union";
		// European blue card H
		conversions[EID_VWR_LANG_FR][19] = "H. Carte bleue europ\u00e9enne";

		conversions[EID_VWR_LANG_NL][1] = "IDENTITEITSKAART";
		conversions[EID_VWR_LANG_NL][6] = "KIDS KAART";
		conversions[EID_VWR_LANG_NL][7] = "OPSTARTKAART";
		conversions[EID_VWR_LANG_NL][8] = "HABILITATIEKAART";
		// Foreigner A";
		conversions[EID_VWR_LANG_NL][11] = "Bewijs van inschrijving in het vreemdelingenregister;Tijdelijk verblijf";
		// Foreigner B";
		conversions[EID_VWR_LANG_NL][12] = "Bewijs van inschrijving in het vreemdelingenregister";
		// Foreigner C";
		conversions[EID_VWR_LANG_NL][13] = "Identiteitskaart voor vreemdeling";
		// Foreigner D";
		conversions[EID_VWR_LANG_NL][14] = "EG-Langdurig ingezetene";
		// Foreigner E";
		conversions[EID_VWR_LANG_NL][15] = "(Verblijfs)kaart van een onderdaan van een lidstaat der EEG;Verklaring van inschrijving";
		// Foreigner E+";
		conversions[EID_VWR_LANG_NL][16] = "Document ter staving van duurzaam verblijf van een EU onderdaan";
		// Foreigner F";
		conversions[EID_VWR_LANG_NL][17] = "Kaart voor niet-EU familieleden van een EU-onderdaan;of van een Belg;Verblijfskaart van een familielid van een burger van de Unie";
		// Foreigner F+";
		conversions[EID_VWR_LANG_NL][18] = "Duurzame verblijfskaart van een familielid van een burger van de Unie";
		// European blue card H";
		conversions[EID_VWR_LANG_NL][19] = "H. Europese blauwe kaart";
	}
}

std::string DocTypeConvertor::convert(const char* normal) {
	if(conversions.count(target_) > 0) {
		std::stringstream stream(normal);
		int i;
		stream >> i;
		if(conversions[target_].count(i) > 0) {
			return conversions[target_][i];
		}
	}
	return normal;
}
