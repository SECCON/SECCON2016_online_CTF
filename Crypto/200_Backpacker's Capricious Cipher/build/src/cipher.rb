require 'json'

N = 128
P = 79228162514264337593543950319

def multiply(a, b)
  r = []
  a.each do |t|
    b.each do |s|
      r.push [t[0] * s[0] % P, t[1] + s[1]]
    end
  end
  r
end

def add(a, b)
  a + b
end

def normalize(a)
  r = []
  a.map{|a,b| [a, b.sort] }.group_by{|a,b| b}.each do |g,p|
    r.push [p.map{|a,b|a}.inject(:+) % P, g]
  end
  r.sort_by{|a,b| b}
end

def gen_private_key
  key = Array.new(N) { rand(P) }
  priv_key = Array.new(N) { rand(2) }
  sum = priv_key.zip(key).map{|a,b|a*b}.inject(:+) % P
  pub_key = [sum, key]
  return [pub_key, priv_key]
end

def encrypt(message, pub_key)
  fail 'Message too long' unless 0 <= message && message < P
  r = [[message, []]]
  t = [[-pub_key[0], []]] + pub_key[1].map.with_index {|a,i| [[a, [i]]]}.inject(:+)

  N.times do |i|
    r = normalize(add(r, multiply([[rand(P), [i]], [rand(P), []]], t)))
  end

  N.times do |i|
    p = rand(P)
    r = normalize(add(r, [[p, [i,i]], [-p, [i]]]))
  end
  r
end

def decrypt(enc, priv_key)
  ret = 0
  enc.each do |c, v|
    mul = (v == []) ? 1 : v.map{|a|priv_key[a]}.inject(:*)
    ret = (ret + c * mul) % P
  end
  ret
end

message = File.read("flag.txt").unpack("H*")[0].to_i(16)
pub_key, priv_key = gen_private_key
enc = encrypt(message, pub_key)
File.write("pub_key.txt", pub_key.to_json)
File.write("priv_key.txt", priv_key.to_json)
File.write("enc.txt", enc.to_json)
fail 'Encryption Failed' unless message == decrypt(enc, priv_key)
