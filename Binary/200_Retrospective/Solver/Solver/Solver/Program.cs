using System;
using System.Collections.Generic;
using System.Linq;
using System.Security.Cryptography;
using System.Text;
using System.Threading.Tasks;

namespace Solver
{
    class Program
    {
        static limited_char[] cache;

        static string solve()
        {
RESCAN:

            var signature = 0x620F3671;
            var str = new limited_char[28];
            var sum = (long)long.MaxValue;
            var maskswitch = false;

            var rnd = new Random();

            for (int i = 0; i < str.Length; i++)
                str[i] = 0;

            for (int i = 0; i < str.Length - 1; i++)
            {
                str[i] = new limited_char((char)rnd.Next((int)' ', (int)'~'));
            }

            for (int i = 0; i < cache.Length; i++)
                str[i] = cache[i];

            if (cache.Length < 16)
            {
                maskswitch = true;

                str[16] = '_';
                str[17] = 'P';
                str[18] = '_';
            }

            do
            {
                var act = rnd.Next(cache.Length, str.Length - 1 - (maskswitch ? 3 : 0));
                if (act >= 16) act += (maskswitch ? 3 : 0);

                var bck = str[act];
                var bck2 = sum;

                if (sum < signature)
                {
                    if (act <= str.Select((e, i) => new { Index = i, Element = e }).First(e => e.Element == 0x0).Index)
                        str[act]++;
                }
                else if (sum > signature)
                {
                    str[act]--;
                    if (str[act + 1] != 0 && str[act] == 0x0)
                    {
                        str[act]++;
                    }
                }

                {
                    var n = str.Select((e, i) => new { Index = i, Element = e }).First(e => e.Element == 0x0).Index;
                    str[n] = '}';
                    sum = str.Select((e, i) => e * (long)Math.Pow(2, i)).Sum();
                    str[n] = 0;
                }
                if ((Math.Abs(bck2 - signature) + 0x1000) <= Math.Abs(sum - signature))
                {
                    str[act] = bck;
                    continue;
                }
            }
            while (sum != signature);

            if (str.Skip(14).Take(2).Select((e, i) => (long)e * (long)Math.Pow(4, i)).Sum() == 350 && str[16] == '_' && str[17] == 'P' && str[18] == '_')
            { }
            else
            {
                goto RESCAN;
            }
            var ret = new string(str.TakeWhile(e => e != 0).Select(e => (char)e).ToArray()).Trim() + "}";

            sum = ret.Select((j, i) => j * (long)Math.Pow(2, i)).Sum();

            return ret;
        }

        static void Main(string[] args)
        {
            cache = "SECCON{LEGACY_".ToCharArray().Select(e => (limited_char)e).ToArray();

            var time = DateTime.Now;

            Console.WriteLine("BEGIN : " + time);

            {
                Console.Write("1st prediction...");

                var ret = new List<string>();

                Parallel.For(0, 20, i =>
                  {
                      ret.Add(solve());
                  });
                ret.Sort();

                Console.WriteLine("Done : " + (DateTime.Now - time));

                var z = ret.Select(e => e.Substring(0, 19)).GroupBy(e => e).ToDictionary(e => e.Key, e => e.Count()).OrderByDescending(e => e.Value);

                foreach(var e in z)
                {
                    Console.WriteLine(e.Key + " --- " + e.Value);
                }

                Console.WriteLine("Choose : " + z.First().Key);

                cache = z.First().Key.Select(e => (limited_char)e).ToArray();
            }
            {
                string ret = "";

                Console.WriteLine();
                Console.Write("2nd prediction...");

                while (true)
                {
                    ret = solve();
                    var hash = BitConverter.ToString(SHA1Managed.Create().ComputeHash(Encoding.ASCII.GetBytes(ret))).Replace("-", "");
                    if (hash == "8B292F1A9C4631B3E13CD49C64EF74540352D0C0")
                        break;
                }

                Console.WriteLine("SOLVED : " + (DateTime.Now - time));
                Console.WriteLine();
                Console.WriteLine("RET : " + ret);
            }

            Console.WriteLine();
        }
    }
}
