package be.belgium.eid;

import java.util.*;

public class LabelsBundle_en
    extends ListResourceBundle
{
  static final Object[][] contents =
  {
      {"AppLabel", "A Web Application wants to access the card !"},
      {"Function", "Function:"},
      {"URL", "URL:"},
      {"Accept", "Do you want to accept it ?"},
      {"Yes", "Yes"},
      {"No", "No"},
      {"Title", "Access Confirmation eID Card"},
      {"ReadPic", "Read Picture Data"},
      {"ReadID", "Read Identity Data"},
      {"ReadAddr", "Read Address Data"},
      {"ReadRaw", "Read Raw Data"}
  };

  public Object[][] getContents()
  {
    return contents;
  }
}
