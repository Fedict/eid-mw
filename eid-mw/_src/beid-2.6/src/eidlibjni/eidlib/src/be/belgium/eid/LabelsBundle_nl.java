package be.belgium.eid;

import java.util.*;

public class LabelsBundle_nl extends ListResourceBundle
{
  static final Object[][] contents =
  {
      {"AppLabel", "Een web applicatie vraagt toegang naar de kaart !"},
      {"Function", "Functie:"},
      {"URL", "URL:"},
      {"Accept", "Wil je verder gaan ?"},
      {"Yes", "Ja"},
      {"No", "Nee"},
      {"Title", "Confirmatie toegang eID kaart"},
      {"ReadPic", "Lezen foto gegevens"},
      {"ReadID", "Lezen identiteit gegevens"},
      {"ReadAddr", "Lezen adres gegevens"},
      {"ReadRaw", "Lezen onbewerkte gegevens"}
  };

  public Object[][] getContents()
  {
    return contents;
  }
}
