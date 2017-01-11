using System;
using System.Collections.Generic;
using System.Linq;
using System.Text;
using System.Threading.Tasks;

namespace Solver
{
    class limited_char
    {
        private char _current;

        public limited_char(char i)
        {
            _current = i;
        }

        public static implicit operator char(limited_char c)
        {
            if (c == null)
                return (char)0;
            else
                return c._current;
        }

        public static implicit operator int(limited_char c)
        {
            if (c == null)
                return 0;
            else
                return c._current;
        }

        public static implicit operator limited_char(char c)
        {
            return new limited_char(c);
        }

        public static implicit operator limited_char(int c)
        {
            return new limited_char((char)c);
        }

        public override string ToString()
        {
            return this._current.ToString();
        }

        public static limited_char operator ++(limited_char c)
        {
            var rc = c._current + 1;

            if (rc > 0x5a)
                rc = 0x5f;

            if (rc > 0 && rc < 0x41)
                rc = 0x41;

            return new limited_char((char)rc);
        }
        public static limited_char operator --(limited_char c)
        {
            var rc = c._current - 1;

            if (rc > 0x5a)
                rc = 0x5a;

            if (rc < 0x41)
                rc = 0x0;

            return new limited_char((char)rc);
        }
    }
}
