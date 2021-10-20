#include "cardAment1conv.h"

std::map < eid_vwr_graphvers, std::map < eid_vwr_langs, std::map <EID_STRING, EID_STRING> > >CardAMention1Convertor::conversions;

CardAMention1Convertor::CardAMention1Convertor() {
	if(conversions.size() == 0)
	{


		conversions[EID_VWR_GRAPH_VERSION_NONE][EID_VWR_LANG_DE][TEXT("A")] = TEXT("Saisonarbeitnehmer");
		conversions[EID_VWR_GRAPH_VERSION_NONE][EID_VWR_LANG_EN][TEXT("A")] = TEXT("Seasonal worker");
		conversions[EID_VWR_GRAPH_VERSION_NONE][EID_VWR_LANG_FR][TEXT("A")] = TEXT("travailleur saisonnier");
		conversions[EID_VWR_GRAPH_VERSION_NONE][EID_VWR_LANG_NL][TEXT("A")] = TEXT("seizoensarbeider");

		conversions[EID_VWR_GRAPH_VERSION_NONE][EID_VWR_LANG_DE][TEXT("D")] = TEXT("Student");
		conversions[EID_VWR_GRAPH_VERSION_NONE][EID_VWR_LANG_EN][TEXT("D")] = TEXT("Student");
		conversions[EID_VWR_GRAPH_VERSION_NONE][EID_VWR_LANG_FR][TEXT("D")] = TEXT("\u00E9tudiant");
		conversions[EID_VWR_GRAPH_VERSION_NONE][EID_VWR_LANG_NL][TEXT("D")] = TEXT("student");

		conversions[EID_VWR_GRAPH_VERSION_NONE][EID_VWR_LANG_DE][TEXT("E")] = TEXT("Sch\u00FCler");
		conversions[EID_VWR_GRAPH_VERSION_NONE][EID_VWR_LANG_EN][TEXT("E")] = TEXT("Pupil");
		conversions[EID_VWR_GRAPH_VERSION_NONE][EID_VWR_LANG_FR][TEXT("E")] = TEXT("\u00E9l\u00E8ve");
		conversions[EID_VWR_GRAPH_VERSION_NONE][EID_VWR_LANG_NL][TEXT("E")] = TEXT("scholier");

		conversions[EID_VWR_GRAPH_VERSION_NONE][EID_VWR_LANG_DE][TEXT("F")] = TEXT("Praktikant");
		conversions[EID_VWR_GRAPH_VERSION_NONE][EID_VWR_LANG_EN][TEXT("F")] = TEXT("Intern");
		conversions[EID_VWR_GRAPH_VERSION_NONE][EID_VWR_LANG_FR][TEXT("F")] = TEXT("stagiair");
		conversions[EID_VWR_GRAPH_VERSION_NONE][EID_VWR_LANG_NL][TEXT("F")] = TEXT("stagiaire");

		conversions[EID_VWR_GRAPH_VERSION_NONE][EID_VWR_LANG_DE][TEXT("G")] = TEXT("Freiwilliger");
		conversions[EID_VWR_GRAPH_VERSION_NONE][EID_VWR_LANG_EN][TEXT("G")] = TEXT("Volunteer");
		conversions[EID_VWR_GRAPH_VERSION_NONE][EID_VWR_LANG_FR][TEXT("G")] = TEXT("volontaire");
		conversions[EID_VWR_GRAPH_VERSION_NONE][EID_VWR_LANG_NL][TEXT("G")] = TEXT("vrijwilliger");

		conversions[EID_VWR_GRAPH_VERSION_NONE][EID_VWR_LANG_DE][TEXT("H")] = TEXT("Au-pair-Kraft");
		conversions[EID_VWR_GRAPH_VERSION_NONE][EID_VWR_LANG_EN][TEXT("H")] = TEXT("Au pair");
		conversions[EID_VWR_GRAPH_VERSION_NONE][EID_VWR_LANG_FR][TEXT("H")] = TEXT("jeune au pair");
		conversions[EID_VWR_GRAPH_VERSION_NONE][EID_VWR_LANG_NL][TEXT("H")] = TEXT("au pair");

		conversions[EID_VWR_GRAPH_VERSION_NONE][EID_VWR_LANG_DE][TEXT("I")] = TEXT("Forscher-Mobilit\u00E4t");
		conversions[EID_VWR_GRAPH_VERSION_NONE][EID_VWR_LANG_EN][TEXT("I")] = TEXT("Researcher mobility");
		conversions[EID_VWR_GRAPH_VERSION_NONE][EID_VWR_LANG_FR][TEXT("I")] = TEXT("Mobilit\u00E9 de chercheur");
		conversions[EID_VWR_GRAPH_VERSION_NONE][EID_VWR_LANG_NL][TEXT("I")] = TEXT("onderzoekersmobiliteit");

		conversions[EID_VWR_GRAPH_VERSION_NONE][EID_VWR_LANG_DE][TEXT("J")] = TEXT("Job Suchen");
		conversions[EID_VWR_GRAPH_VERSION_NONE][EID_VWR_LANG_EN][TEXT("J")] = TEXT("Job search");
		conversions[EID_VWR_GRAPH_VERSION_NONE][EID_VWR_LANG_FR][TEXT("J")] = TEXT("Recherche emploi");
		conversions[EID_VWR_GRAPH_VERSION_NONE][EID_VWR_LANG_NL][TEXT("J")] = TEXT("Werk zoeken");



		conversions[EID_VWR_GRAPH_VERSION_EIGHT][EID_VWR_LANG_DE][TEXT("A")] = TEXT("SAISONARBEITER");
		conversions[EID_VWR_GRAPH_VERSION_EIGHT][EID_VWR_LANG_EN][TEXT("A")] = TEXT("SEASONAL WORKER");
		conversions[EID_VWR_GRAPH_VERSION_EIGHT][EID_VWR_LANG_FR][TEXT("A")] = TEXT("SAISONNIER");
		conversions[EID_VWR_GRAPH_VERSION_EIGHT][EID_VWR_LANG_NL][TEXT("A")] = TEXT("SEIZOENARBEIDER");

		conversions[EID_VWR_GRAPH_VERSION_EIGHT][EID_VWR_LANG_DE][TEXT("D")] = TEXT("STUDENT");
		conversions[EID_VWR_GRAPH_VERSION_EIGHT][EID_VWR_LANG_EN][TEXT("D")] = TEXT("STUDENT");
		conversions[EID_VWR_GRAPH_VERSION_EIGHT][EID_VWR_LANG_FR][TEXT("D")] = TEXT("ETUDIANT");
		conversions[EID_VWR_GRAPH_VERSION_EIGHT][EID_VWR_LANG_NL][TEXT("D")] = TEXT("STUDENT");

		conversions[EID_VWR_GRAPH_VERSION_EIGHT][EID_VWR_LANG_DE][TEXT("E")] = TEXT("SCHULER");
		conversions[EID_VWR_GRAPH_VERSION_EIGHT][EID_VWR_LANG_EN][TEXT("E")] = TEXT("PUPIL");
		conversions[EID_VWR_GRAPH_VERSION_EIGHT][EID_VWR_LANG_FR][TEXT("E")] = TEXT("ELEVE");
		conversions[EID_VWR_GRAPH_VERSION_EIGHT][EID_VWR_LANG_NL][TEXT("E")] = TEXT("SCHOLIER");

		conversions[EID_VWR_GRAPH_VERSION_EIGHT][EID_VWR_LANG_DE][TEXT("F")] = TEXT("PRAKTIKANT");
		conversions[EID_VWR_GRAPH_VERSION_EIGHT][EID_VWR_LANG_EN][TEXT("F")] = TEXT("INTERN");
		conversions[EID_VWR_GRAPH_VERSION_EIGHT][EID_VWR_LANG_FR][TEXT("F")] = TEXT("STAGIAIRE");
		conversions[EID_VWR_GRAPH_VERSION_EIGHT][EID_VWR_LANG_NL][TEXT("F")] = TEXT("STAGIAIR");

		conversions[EID_VWR_GRAPH_VERSION_EIGHT][EID_VWR_LANG_DE][TEXT("G")] = TEXT("FREIWILLIGER");
		conversions[EID_VWR_GRAPH_VERSION_EIGHT][EID_VWR_LANG_EN][TEXT("G")] = TEXT("VOLUNTEER");
		conversions[EID_VWR_GRAPH_VERSION_EIGHT][EID_VWR_LANG_FR][TEXT("G")] = TEXT("VOLONTAIRE");
		conversions[EID_VWR_GRAPH_VERSION_EIGHT][EID_VWR_LANG_NL][TEXT("G")] = TEXT("VRIJWILLIGER");

		conversions[EID_VWR_GRAPH_VERSION_EIGHT][EID_VWR_LANG_DE][TEXT("H")] = TEXT("AU-PAIR-KRAFT");
		conversions[EID_VWR_GRAPH_VERSION_EIGHT][EID_VWR_LANG_EN][TEXT("H")] = TEXT("AU PAIR");
		conversions[EID_VWR_GRAPH_VERSION_EIGHT][EID_VWR_LANG_FR][TEXT("H")] = TEXT("JEUNE AU PAIR");
		conversions[EID_VWR_GRAPH_VERSION_EIGHT][EID_VWR_LANG_NL][TEXT("H")] = TEXT("AU PAIR");

		conversions[EID_VWR_GRAPH_VERSION_EIGHT][EID_VWR_LANG_DE][TEXT("I")] = TEXT("FORSCHER-MOBILITAT");
		conversions[EID_VWR_GRAPH_VERSION_EIGHT][EID_VWR_LANG_EN][TEXT("I")] = TEXT("RESEARCHER MOBILITY");
		conversions[EID_VWR_GRAPH_VERSION_EIGHT][EID_VWR_LANG_FR][TEXT("I")] = TEXT("MOBILITE DE CHERCHEUR");
		conversions[EID_VWR_GRAPH_VERSION_EIGHT][EID_VWR_LANG_NL][TEXT("I")] = TEXT("ONDERZOEKERSMOBILITEIT");

		conversions[EID_VWR_GRAPH_VERSION_EIGHT][EID_VWR_LANG_DE][TEXT("J")] = TEXT("JOB SUCHEN");
		conversions[EID_VWR_GRAPH_VERSION_EIGHT][EID_VWR_LANG_EN][TEXT("J")] = TEXT("JOB SEARCH");
		conversions[EID_VWR_GRAPH_VERSION_EIGHT][EID_VWR_LANG_FR][TEXT("J")] = TEXT("RECHERCHE EMPLOI");
		conversions[EID_VWR_GRAPH_VERSION_EIGHT][EID_VWR_LANG_NL][TEXT("J")] = TEXT("WERK ZOEKEN");
	}
}

EID_STRING CardAMention1Convertor::convert(const void *normal) {
	if (conversions.count(graphvers_) > 0)
	{
		if (conversions[graphvers_].count(target_) > 0)
		{
			EID_STRING norm = (EID_CHAR*)normal;
			if (conversions[graphvers_][target_].count(norm) > 0)
			{
				return conversions[graphvers_][target_][norm];
			}
		}
	}
	return (EID_CHAR*)normal;
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
