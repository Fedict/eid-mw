package be.belgium.eid;

import java.util.*;

public class LabelsBundle_fr extends ListResourceBundle
{
  static final Object[][] contents =
    {
        {"AppLabel", "Une application Web veut accéder à la carte !"},
        {"Function", "Fonction:"},
        {"URL", "URL:"},
        {"Accept", "Voulez-vous l'accepter ?"},
        {"Yes", "Oui"},
        {"No", "Non"},
        {"Title", "Confirmation d'accès à la carte eID"},
        {"ReadPic", "Lire la photo"},
        {"ReadID", "Lire les données d'identité"},
        {"ReadAddr", "Lire l'adresse"},
        {"ReadRaw", "Lire des données non spécifiées"}
    };

    public Object[][] getContents()
    {
      return contents;
    }
}
