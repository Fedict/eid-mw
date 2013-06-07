using System;
using System.Collections.Generic;
using System.Linq;
using System.Text;

namespace beidmdrvTest
{
  class Program
  {
    public static void Main(String[] args)
    {
      Console.WriteLine("Press a key to start the test.");
      System.Console.Read();

      SignTests signTest = new SignTests();
      signTest.TestSignWithSignatureKey();

      Console.WriteLine("Press a key to end the test.");
      System.Console.Read();
    }
  }
}
