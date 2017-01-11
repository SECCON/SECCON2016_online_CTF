class CZ:
    def __init__(self, N):
        self.N = N

    def __call__(self, x, y):
        return CZimpl(x, y, self)

class CZimpl:
    def __init__(self, x, y, klass):
        N = klass.N
        self.C = klass
        self.x = x % N
        self.y = y % N

    def __neg__(self):
        return self.C(-self.x, -self.y)
  
    def __add__(self, other):
        return self.C(self.x + other.x,
                      self.y + other.y)

    def __sub__(self, other):
        return self.C(self.x - other.x,
                      self.y - other.y)

    def __mul__(self, other):
        if isinstance(other, CZimpl):
            return self.C(self.x * other.x - self.y * other.y,
                          self.y * other.x + self.x * other.y)
        else:
            return self.C(self.x * other, self.y * other)

    def __pow__(self, k):
        r = self.C(1, 0)
        y = self
        while k > 0:
            if k & 1 != 0:
                r *= y
            y *= y
            k >>= 1
        return r

    def __repr__(self):
        return "[%d, %d]" % (self.x, self.y)
