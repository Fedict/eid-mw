package be.belgium.eid;

import java.util.*;

public class LabelsBundle_de extends ListResourceBundle
{
  static final Object[][] contents =
  {
      {"AppLabel", "Eine Netzanwendung möchte die Karte zugänglich machen !"},
      {"Function", "Funktion:"},
      {"URL", "URL:"},
      {"Accept", "Möchten Sie es annehmen ?"},
      {"Yes", "Ja"},
      {"No", "Nein"},
      {"Title", "eID Karte Zugang Bestätigung"},
      {"ReadPic", "Lesen Sie Abbildung Daten"},
      {"ReadID", "Lesen Sie Identität Daten"},
      {"ReadAddr", "Lesen Sie Adresse Daten"},
      {"ReadRaw", "Lesen Sie rohe Daten"}
  };

  public Object[][] getContents()
  {
    return contents;
  }
}
