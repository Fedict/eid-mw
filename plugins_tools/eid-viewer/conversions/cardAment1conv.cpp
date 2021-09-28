#include "cardAment1conv.h"

std::map < eid_vwr_graphvers, std::map < eid_vwr_langs, std::map <char, EID_STRING> > >CardAMention1Convertor::conversions;

CardAMention1Convertor::CardAMention1Convertor() {
	if(conversions.size() == 0)
	{


		conversions[EID_VWR_GRAPH_VERSION_NONE][EID_VWR_LANG_DE]['A'] = TEXT("Saisonarbeitnehmer");
		conversions[EID_VWR_GRAPH_VERSION_NONE][EID_VWR_LANG_EN]['A'] = TEXT("Seasonal worker");
		conversions[EID_VWR_GRAPH_VERSION_NONE][EID_VWR_LANG_FR]['A'] = TEXT("travailleur saisonnier");
		conversions[EID_VWR_GRAPH_VERSION_NONE][EID_VWR_LANG_NL]['A'] = TEXT("seizoensarbeider");

		conversions[EID_VWR_GRAPH_VERSION_NONE][EID_VWR_LANG_DE]['D'] = TEXT("Student");
		conversions[EID_VWR_GRAPH_VERSION_NONE][EID_VWR_LANG_EN]['D'] = TEXT("Student");
		conversions[EID_VWR_GRAPH_VERSION_NONE][EID_VWR_LANG_FR]['D'] = TEXT("\u00E9tudiant");
		conversions[EID_VWR_GRAPH_VERSION_NONE][EID_VWR_LANG_NL]['D'] = TEXT("student");

		conversions[EID_VWR_GRAPH_VERSION_NONE][EID_VWR_LANG_DE]['E'] = TEXT("Sch\u00FCler");
		conversions[EID_VWR_GRAPH_VERSION_NONE][EID_VWR_LANG_EN]['E'] = TEXT("Pupil");
		conversions[EID_VWR_GRAPH_VERSION_NONE][EID_VWR_LANG_FR]['E'] = TEXT("\u00E9l\u00E8ve");
		conversions[EID_VWR_GRAPH_VERSION_NONE][EID_VWR_LANG_NL]['E'] = TEXT("scholier");

		conversions[EID_VWR_GRAPH_VERSION_NONE][EID_VWR_LANG_DE]['F'] = TEXT("Praktikant");
		conversions[EID_VWR_GRAPH_VERSION_NONE][EID_VWR_LANG_EN]['F'] = TEXT("Intern");
		conversions[EID_VWR_GRAPH_VERSION_NONE][EID_VWR_LANG_FR]['F'] = TEXT("stagiair");
		conversions[EID_VWR_GRAPH_VERSION_NONE][EID_VWR_LANG_NL]['F'] = TEXT("stagiaire");

		conversions[EID_VWR_GRAPH_VERSION_NONE][EID_VWR_LANG_DE]['G'] = TEXT("Freiwilliger");
		conversions[EID_VWR_GRAPH_VERSION_NONE][EID_VWR_LANG_EN]['G'] = TEXT("Volunteer");
		conversions[EID_VWR_GRAPH_VERSION_NONE][EID_VWR_LANG_FR]['G'] = TEXT("volontaire");
		conversions[EID_VWR_GRAPH_VERSION_NONE][EID_VWR_LANG_NL]['G'] = TEXT("vrijwilliger");

		conversions[EID_VWR_GRAPH_VERSION_NONE][EID_VWR_LANG_DE]['H'] = TEXT("Au-pair-Kraft");
		conversions[EID_VWR_GRAPH_VERSION_NONE][EID_VWR_LANG_EN]['H'] = TEXT("Au pair");
		conversions[EID_VWR_GRAPH_VERSION_NONE][EID_VWR_LANG_FR]['H'] = TEXT("jeune au pair");
		conversions[EID_VWR_GRAPH_VERSION_NONE][EID_VWR_LANG_NL]['H'] = TEXT("au pair");

		conversions[EID_VWR_GRAPH_VERSION_NONE][EID_VWR_LANG_DE]['I'] = TEXT("Forscher-Mobilit\u00E4t");
		conversions[EID_VWR_GRAPH_VERSION_NONE][EID_VWR_LANG_EN]['I'] = TEXT("Researcher mobility");
		conversions[EID_VWR_GRAPH_VERSION_NONE][EID_VWR_LANG_FR]['I'] = TEXT("Mobilit\u00E9 de chercheur");
		conversions[EID_VWR_GRAPH_VERSION_NONE][EID_VWR_LANG_NL]['I'] = TEXT("onderzoekersmobiliteit");

		conversions[EID_VWR_GRAPH_VERSION_NONE][EID_VWR_LANG_DE]['J'] = TEXT("Job Suchen");
		conversions[EID_VWR_GRAPH_VERSION_NONE][EID_VWR_LANG_EN]['J'] = TEXT("Job search");
		conversions[EID_VWR_GRAPH_VERSION_NONE][EID_VWR_LANG_FR]['J'] = TEXT("Recherche emploi");
		conversions[EID_VWR_GRAPH_VERSION_NONE][EID_VWR_LANG_NL]['J'] = TEXT("Werk zoeken");



		conversions[EID_VWR_GRAPH_VERSION_EIGHT][EID_VWR_LANG_DE]['A'] = TEXT("SAISONARBEITER");
		conversions[EID_VWR_GRAPH_VERSION_EIGHT][EID_VWR_LANG_EN]['A'] = TEXT("SEASONAL WORKER");
		conversions[EID_VWR_GRAPH_VERSION_EIGHT][EID_VWR_LANG_FR]['A'] = TEXT("SAISONNIER");
		conversions[EID_VWR_GRAPH_VERSION_EIGHT][EID_VWR_LANG_NL]['A'] = TEXT("SEIZOENARBEIDER");

		conversions[EID_VWR_GRAPH_VERSION_EIGHT][EID_VWR_LANG_DE]['D'] = TEXT("STUDENT");
		conversions[EID_VWR_GRAPH_VERSION_EIGHT][EID_VWR_LANG_EN]['D'] = TEXT("STUDENT");
		conversions[EID_VWR_GRAPH_VERSION_EIGHT][EID_VWR_LANG_FR]['D'] = TEXT("ETUDIANT");
		conversions[EID_VWR_GRAPH_VERSION_EIGHT][EID_VWR_LANG_NL]['D'] = TEXT("STUDENT");

		conversions[EID_VWR_GRAPH_VERSION_EIGHT][EID_VWR_LANG_DE]['E'] = TEXT("SCHULER");
		conversions[EID_VWR_GRAPH_VERSION_EIGHT][EID_VWR_LANG_EN]['E'] = TEXT("PUPIL");
		conversions[EID_VWR_GRAPH_VERSION_EIGHT][EID_VWR_LANG_FR]['E'] = TEXT("ELEVE");
		conversions[EID_VWR_GRAPH_VERSION_EIGHT][EID_VWR_LANG_NL]['E'] = TEXT("SCHOLIER");

		conversions[EID_VWR_GRAPH_VERSION_EIGHT][EID_VWR_LANG_DE]['F'] = TEXT("PRAKTIKANT");
		conversions[EID_VWR_GRAPH_VERSION_EIGHT][EID_VWR_LANG_EN]['F'] = TEXT("INTERN");
		conversions[EID_VWR_GRAPH_VERSION_EIGHT][EID_VWR_LANG_FR]['F'] = TEXT("STAGIAIRE");
		conversions[EID_VWR_GRAPH_VERSION_EIGHT][EID_VWR_LANG_NL]['F'] = TEXT("STAGIAIR");

		conversions[EID_VWR_GRAPH_VERSION_EIGHT][EID_VWR_LANG_DE]['G'] = TEXT("FREIWILLIGER");
		conversions[EID_VWR_GRAPH_VERSION_EIGHT][EID_VWR_LANG_EN]['G'] = TEXT("VOLUNTEER");
		conversions[EID_VWR_GRAPH_VERSION_EIGHT][EID_VWR_LANG_FR]['G'] = TEXT("VOLONTAIRE");
		conversions[EID_VWR_GRAPH_VERSION_EIGHT][EID_VWR_LANG_NL]['G'] = TEXT("VRIJWILLIGER");

		conversions[EID_VWR_GRAPH_VERSION_EIGHT][EID_VWR_LANG_DE]['H'] = TEXT("AU-PAIR-KRAFT");
		conversions[EID_VWR_GRAPH_VERSION_EIGHT][EID_VWR_LANG_EN]['H'] = TEXT("AU PAIR");
		conversions[EID_VWR_GRAPH_VERSION_EIGHT][EID_VWR_LANG_FR]['H'] = TEXT("JEUNE AU PAIR");
		conversions[EID_VWR_GRAPH_VERSION_EIGHT][EID_VWR_LANG_NL]['H'] = TEXT("AU PAIR");

		conversions[EID_VWR_GRAPH_VERSION_EIGHT][EID_VWR_LANG_DE]['I'] = TEXT("FORSCHER-MOBILITAT");
		conversions[EID_VWR_GRAPH_VERSION_EIGHT][EID_VWR_LANG_EN]['I'] = TEXT("RESEARCHER MOBILITY");
		conversions[EID_VWR_GRAPH_VERSION_EIGHT][EID_VWR_LANG_FR]['I'] = TEXT("MOBILITE DE CHERCHEUR");
		conversions[EID_VWR_GRAPH_VERSION_EIGHT][EID_VWR_LANG_NL]['I'] = TEXT("ONDERZOEKERSMOBILITEIT");

		conversions[EID_VWR_GRAPH_VERSION_EIGHT][EID_VWR_LANG_DE]['J'] = TEXT("JOB SUCHEN");
		conversions[EID_VWR_GRAPH_VERSION_EIGHT][EID_VWR_LANG_EN]['J'] = TEXT("JOB SEARCH");
		conversions[EID_VWR_GRAPH_VERSION_EIGHT][EID_VWR_LANG_FR]['J'] = TEXT("RECHERCHE EMPLOI");
		conversions[EID_VWR_GRAPH_VERSION_EIGHT][EID_VWR_LANG_NL]['J'] = TEXT("WERK ZOEKEN");
	}
}

EID_STRING CardAMention1Convertor::convert(const void *normal) {
	const char *norm = (const char*)normal;
	return conversions[graphvers_][target_][*norm];
}

std::map < EID_STRING, EID_STRING > XmlCardAMention1Convertor::conversions;

XmlCardAMention1Convertor::XmlCardAMention1Convertor() {
	if(conversions.size() == 0)
	{
		conversions[TEXT("A")] = TEXT("Seasonal worker");
		conversions[TEXT("D")] = TEXT("Student");
		conversions[TEXT("E")] = TEXT("Pupil");
		conversions[TEXT("F")] = TEXT("Intern");
		conversions[TEXT("G")] = TEXT("Volunteer");
		conversions[TEXT("H")] = TEXT("Au pair");
		conversions[TEXT("I")] = TEXT("Researcher mobility");
		conversions[TEXT("J")] = TEXT("Job search");

		conversions[TEXT("Seasonal worker")] = TEXT("A");
		conversions[TEXT("Student")] = TEXT("D");
		conversions[TEXT("Pupil")] = TEXT("E");
		conversions[TEXT("Intern")] = TEXT("F");
		conversions[TEXT("Volunteer")] = TEXT("G");
		conversions[TEXT("Au pair")] = TEXT("H");
		conversions[TEXT("Researcher mobility")] = TEXT("I");
		conversions[TEXT("Job search")] = TEXT("J");
	}
}

EID_STRING XmlCardAMention1Convertor::convert(const void *normal) {
	EID_STRING norm((const EID_CHAR *) normal);
	return conversions[norm];
}
