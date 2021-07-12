import struct

class cursor:
    def __init__(self, buf):
        self._buf = buf
        self._p = 0

    def read(self, n):
        val = self._buf[self._p : self._p + n]
        assert(len(val) == n)
        self._p += n
        return val
    
    def peek(self, n):
        val = self._buf[self._p : self._p + n]
        assert(len(val) == n)
        return val

    def read_s32le(self):
        return struct.unpack("<i", self.read(4))[0]

    def read_string(self):
        length = self.read_s32le()
        return self.read(length)

    def read_unknown_s32le(self, expected):
        if type(expected) not in (tuple, list):
            expected = (expected,)
        unknown = self.read_s32le()
        print("unknown = {} ({:#x})".format(unknown, unknown))
        if unknown not in expected:
            s = ["{} ({:#x})".format(_, _) for _ in expected]
            if len(s) > 1:
                s[-1] = "or " + s[-1]
            expected_string = ', '.join(s)
            raise Exception("Expected {}; but got {} ({:#x}) - next few bytes: {}".format(expected_string, unknown, unknown, self.read(15)))

import sys

with open(sys.argv[1], "rb") as f:
# with open("001-aqua-hgru4-5fevv-aaz32-7npj8.DFAM.creature.out", "rb") as f:
# with open("../SpaceAndAllThatIsOutThere.out", "rb") as f:
    data = f.read()

cur = cursor(data)

creaturesarchivemagic = cur.read_s32le()
# print("creaturesarchive magic", creaturesarchivemagic)
assert(creaturesarchivemagic == 0x27)

creaturesarchivetype = cur.read_s32le()
# print("creaturesarchive type", creaturesarchivetype)
assert(creaturesarchivetype == 0x0)

# cur.read_unknown_s32le(expected=0)
unknown_zero = cur.read_s32le()
assert(unknown_zero == 0)
length = cur.read_s32le()
name = cur.read(length)
# print(name) # SkeletalCreature
    
import binascii
import html
types = {}

def binrepr(buf):
    out = ''
    for c in buf:
        if c in b'0123456789abcdefghijklmnopqrstuvwxyzABCDEFGHIJKLMNOPQRSTUVWXYZ!"#$%&\'()*+,-./:;<=>?@[\\]^_`{|}~ ':
            out += chr(c)
        else:
            out += '\\x' + bytes([c]).hex()
    return html.escape(out)

print("""
<style type="text/css">

body {
    word-break: break-all;
}

.object-name {
    // font-size: 0.8em;
    font-weight: bold;
}

.object {
border: 1px solid;
}

.data-length {
font-size: 0.8em;
font-weight: bold;
/* color: rgb(150, 150, 150); */
}

.obst, .oben {
  color: rgb(150, 150, 150);
}

.name {
    font-weight: bold;
    font-size: 0.8em;
}

.string:before {
display:block;
content:"string";
font-weight: bold;
font-size:0.8em;
}

.unknown:before {
display:block;
content:"unknown";
font-weight: bold;
font-size:0.8em;
}

.string {
    background-color: lightblue;
}

.named-value {
    background-color: lightgreen;
}

.raw {
    color: rgb(150, 150, 150);
}

.data, .raw, .obst, .oben {
    font-family: monospace;
}

</style>
""")

print("<div>")

current_span = b''
i = 0

class printerhelper:
    def __init__(self):
        self.depth = 0
        self.inline = False
    def set_inline(self, inline_):
        self.inline = inline_
        if inline_ == False:
            print("<div></div>")
    def data(self, buf):
        if not buf:
            return
        print("<div class=\"data-length\">")
        print("{} bytes".format(len(buf)))
        print("</div>")
        print("<div class=\"data\">")
        print("{}".format(binrepr(buf)))
        print("</div>")
    def unknown_float(self, buf):
        assert(len(buf) == 4)
        value = round(struct.unpack('<f', buf)[0], 4)
        print("<div class=\"unknown\" {}>".format("style=\"display: inline-block\"" if self.inline else ""))
        print("{} <span class=\"raw\">({})</span>".format(value, binrepr(buf)))
        print("</div>")
    def unknown(self, buf):
        if len(buf) == 4:
            value = struct.unpack('<i', buf)[0]
        elif len(buf) == 2:
            value = struct.unpack('<H', buf)[0]
        else:
            assert(len(buf) in (2, 4))
        print("<div class=\"unknown\" {}>".format("style=\"display: inline-block\"" if self.inline else ""))
        print("{} <span class=\"raw\">({})</span>".format(value, binrepr(buf)))
        print("</div>")
    def string(self, s):
        print("<div class=\"string\"{}><span class=\"raw\">{}</span>{}</div>".format("style=\"display: inline-block\"" if self.inline else "", binrepr(struct.pack('<I', len(s))), html.escape(s)))
    def named(self, name, buf):
        print("<div class=\"name\">")
        print("{}".format(name))
        print("</div>")
        print("<div class=\"named-value\">")
        print("{}".format(binrepr(buf)))
        print("</div>")
    def integer(self, name, buf):
        if len(buf) == 4:
            value = struct.unpack('<I', buf)[0]
        elif len(buf) == 2:
            value = struct.unpack('<H', buf)[0]
        else:
            assert(len(data) in (2, 4))
        print("<div class=\"named-value\"{}>".format("style=\"display: inline-block\"" if self.inline else ""))
        print("<div class=\"name\">")
        print("{}".format(name))
        print("</div>")
        print("{} <span class=\"raw\">({})</span>".format(value, binrepr(buf)))
        print("</div>")

printer = printerhelper()

def parse_limb():
    global i
    assert data[i+606:i+606+8] == b'\x04\x00\x00\x00OBST'
    
    printer.set_inline(True)
    for _ in range(5):
        printer.unknown(data[i:i+4])
        i += 4
    printer.set_inline(False)
    
    length = struct.unpack('<I', data[i:i+4])[0]
    script = data[i+4:i+4+length].decode('cp1252')
    printer.string(script)
    i += 4 + length

def parse_brain():
    global i
    assert data[i+16:i+16+8] == b'\x04\x00\x00\x00OBST' or data[i+16:i+16+8] == b'\x04\x00\x00\x00Lobe'

    printer.unknown(data[i:i+4])
    i += 4
    
    printer.integer("number of lobes", data[i:i+4])
    i += 4

def parse_bodypartoverlay():
    global i
    assert data[i+40:i+48] == b'\x04\x00\x00\x00OBEN'
    
    printer.integer("parent index", data[i:i+4])
    i += 4
    printer.unknown(data[i:i+4])
    i += 4
    printer.data(data[i:i+32])
    i += 32
    return

def parse_linguisticfaculty():
    global i
    printer.integer("enabled", data[i:i+4])
    i += 4
    
    for category in ("verbs", "nouns", "adjectives", "other", "modifiers", "subjects", "drives"):
        num_words = struct.unpack('<I', data[i:i+4])[0]
        printer.integer("num " + category, data[i:i+4])
        i += 4
        
        for _ in range(num_words):
            printer.set_inline(True)
            length = struct.unpack('<I', data[i:i+4])[0]
            script = data[i+4:i+4+length].decode('cp1252')
            printer.string(script)
            i += 4 + length
            
            length = struct.unpack('<I', data[i:i+4])[0]
            script = data[i+4:i+4+length].decode('cp1252')
            printer.string(script)
            i += 4 + length
            
            printer.unknown(data[i:i+2])
            i += 2
            printer.unknown(data[i:i+2])
            i += 2
            printer.unknown(data[i:i+4])
            i += 4
            printer.set_inline(False)
    

def parse_musicfaculty():
    global i
    assert data[i+4:i+12] == b'\x04\x00\x00\x00OBEN'
    printer.integer("enabled", data[i:i+4])
    i += 4

def parse_receptor():
    global i
    
    printer.set_inline(True)
    for _ in range(4):
        printer.unknown(data[i:i+4])
        i += 4
    printer.set_inline(False)
    printer.set_inline(True)
    for _ in range(3):
        printer.unknown_float(data[i:i+4])
        i+=4
    printer.set_inline(False)
    printer.set_inline(True)
    for _ in range(3):
        printer.unknown(data[i:i+4])
        i += 4
    printer.set_inline(False)
    
    assert data[i:i+8] == b'\x04\x00\x00\x00OBEN'

def parse_macroscript():
    global i
    
    printer.unknown(data[i:i+4])
    i += 4
    printer.integer("family", data[i:i+4])
    i += 4
    printer.integer("genus", data[i:i+4])
    i += 4
    printer.integer("species", data[i:i+4])
    i += 4
    
    printer.integer("event number", data[i:i+4])
    i += 4

# u32 event_number
# u32 unknown - 0 - EMIT's command group? or something random?
# u32 unknown - 14 - ???
# u32 unknown - 107 - EMIT's command index?
# u32 unknown - 1 - first argument, a raw integer. how does it know?
# u16 unknown - 3 - ? third caos command group? (_P1_ is in group 2, which is the third one...)
# u16 unknown - 303 - _P1_'s command index!
# u16 unknown - 17 - ???
# u32 object index? - 3

def parse_debuginfo():
    global i
    length = struct.unpack('<I', data[i:i+4])[0]
    script = data[i+4:i+4+length].decode('cp1252')
    printer.string(script)
    i += 4 + length
    
    printer.integer("num commands", data[i:i+4])
    i += 4
    
    # if data[i+116:i+116+8] == b'\x04x00x00x00OBEN':
    # for _ in range(29):
    #     print("<div style=\"margin-left: {}px\">{}</div>".format(depth * 10, struct.unpack('<I', data[i:i+4])[0]))
    #     i += 4

while i < len(data):
    if i + 4 < len(data) and data[i:i+4] == b'OBST':
        assert(data[i-4:i] == b'\x04\x00\x00\x00')
        j = i-5
        if data[j] != 0:
            while data[j] != 0:
                j -= 1
            j -= 3
            name_length = struct.unpack('<I', data[j:j+4])[0]
            # print("name length", name_length)
            name = data[j+4:j+4+name_length]
            j-=4
            object_type = struct.unpack('<I', data[j:j+4])[0]
            types[object_type] = name.decode('ascii')
        else:
            j -= 3
            object_type = struct.unpack('<I', data[j:j+4])[0]
        
        j -= 4
        object_index = struct.unpack('<I', data[j:j+4])[0]
        
        
        # j-4 to also include object index
        current_span = current_span[:len(current_span) - (i - j)]
        printer.data(current_span)
        current_span = b''
        
        printer.depth += 1
        
        print("<div style=\"margin-left: {}px\" class=\"object\">".format(printer.depth * 10))
        print("<div class=\"object-name\">{} index={} type={}</div>".format(types[object_type], object_index, object_type))
        print("<div class=\"obst\">".format(printer.depth * 10))
        print("{}".format(binrepr(data[j:i+4])))
        print("</div>")
        # spans.append((j-4, i+4, 'OBST', types[object_type]))
        # print("    " * depth + "{} at {}".format(types[object_type], i))
        i += 4
        
        if types[object_type] == 'DebugInfo':
            parse_debuginfo()
        if types[object_type] == 'MacroScript':
            parse_macroscript()
        if types[object_type] == 'BodyPartOverlay':
            parse_bodypartoverlay()
        if types[object_type] == 'Limb':
            parse_limb()
        if types[object_type] == 'Brain':
            parse_brain()
        if types[object_type] == 'MusicFaculty':
            parse_musicfaculty()
        if types[object_type] == 'LinguisticFaculty':
            parse_linguisticfaculty()
        if types[object_type] == 'Receptor':
            parse_receptor()
        
    elif i + 4 < len(data) and data[i:i+4] == b'OBEN':
        assert(data[i-4:i] == b'\x04\x00\x00\x00')
        current_span = current_span[:len(current_span) - 4]
        printer.data(current_span)
        current_span = b''
        print("<div class=\"oben\">")
        print("{}".format(binrepr(data[i-4:i+4])))
        print("</div>")
        print("</div>") # end of .object
        # print("    " * depth + "end of object")
        printer.depth -= 1
        i += 4
        # spans.append((i-4, i+4, 'OBEN'))
    else:
        current_span += data[i:i+1]
        i += 1

# assert(depth == 1)
printer.data(current_span)

print("</div>")
        
        
# 
# print(types)
# print(spans)
# 
# new_spans = []
# p = 0
# for _ in spans:
#     if _[0] > p:
#         new_spans.append((p, _[0], 'data', data[p:_[0]]))
#     new_spans.append(_)
# 
# # print(new_spans)
# 
# 
# class buffer:
#     def __init__(self, buf_):
#         self._buf = buf_
#     def __repr__(self):
#         return repr(self._buf)
# 
# class genericobject:
#     def __init__(self, name):
#         self.name = name
#         self.data = []
# 
# class parser:
#     def __init__(self, new_spans_):
#         self.p = 0
#         self.new_spans = new_spans_
# 
#     def parse_object(self):
#         print("parse_object")
#         ret = []
#         self.new_spans[self.p][2] == 'OBST'
#         ret.append(self.new_spans[self.p][3])
#         self.p += 1
# 
#         while self.p < len(self.new_spans):
#             assert(self.new_spans[self.p][2] in ('OBST', 'OBEN', 'data'))
#             if self.new_spans[self.p][2] == 'OBST':
#                 ret.append(self.parse_object())
#             elif self.new_spans[self.p][2] == 'OBEN':
#                 self.p += 1
#                 return ret
#             else:
#                 ret.append(buffer(self.new_spans[self.p][3]))
#                 self.p += 1
#         print(ret)
#         assert(False)
# 
#     def parse_toplevel(self):
#         ret = []
#         while self.p < len(self.new_spans):
#             assert(self.new_spans[self.p][2] in ('OBST', 'data'))
#             if self.new_spans[self.p][2] == 'OBST':
#                 ret.append(self.parse_object())
#             else:
#                 ret.append(buffer(self.new_spans[self.p][3]))
#                 self.p += 1
#         return ret
# 
# p = parser(new_spans)    
# print(p.parse_toplevel())

        # print(data[j:i])


exit()





# for world saves



with open("SpaceAndAllThatIsOutThere.out", "rb") as f:
    data = f.read()

cur = cursor(data)

creaturesarchivemagic = cur.read_s32le()
print("creaturesarchive magic", creaturesarchivemagic)
assert(creaturesarchivemagic == 0x27)

creaturesarchivetype = cur.read_s32le()
print("creaturesarchive type", creaturesarchivetype)
assert(creaturesarchivetype == 0x0)

cur.read_unknown_s32le(expected=0)

number = cur.read_s32le()
print("number", number)

for _ in range(number):
    # cur.read_unknown_s32le(expected=1)
    bytes = cur.read(9)
    print("bytes", list(bytes))

cur.read_unknown_s32le(expected=0)
number_of_objects = cur.read_s32le()
print("number of objects", number_of_objects)

cur.read_unknown_s32le(expected=0) # object index
cur.read_unknown_s32le(expected=0) # object type
length = cur.read_s32le()
name = cur.read(length)
print("name", name)

def parse_generic_object():
    # cur.read_unknown_s32le(expected=4) # length of OBST?
    obst = cur.read(8)
    if obst != b'\x04\x00\x00\x00OBST':
        raise Exception("Expected \x04\x00\x00\x00OBST, got {}".format(obst))
    depth = 1
    while True:
        if cur.peek(4) == b'OBST':
            depth += 1
            cur.read(4)
            continue
        elif cur.peek(4) == b'OBEN':
            depth -= 1
            cur.read(4)
            if depth == 0:
                break
            else:
                continue
        else:
            cur.read(1)
    return '<object>'

print(parse_generic_object())
for _ in range(number_of_objects - 1):
    object_index = cur.read_s32le()
    object_depth = cur.read_s32le()
    # print("object index", cur.read_s32le()) 
    # print("object depth", cur.read_s32le())
    parse_generic_object()

# number_of_objects = cur.read_s32le()
# print("number of objects", number_of_objects)

cur.read_unknown_s32le(0x4c5) # number of objects?
cur.read_unknown_s32le(0xa)
cur.read_unknown_s32le(0x718)
cur.read_unknown_s32le(0x12)
cur.read_unknown_s32le(0x12)
print("unknown", cur.read(20))
cur.read_unknown_s32le(0xfa)
cur.read_unknown_s32le(0xfa)
cur.read_unknown_s32le(0xc)
# cur.read_unknown_s32le(0x0)

object_index = cur.read_s32le()
print("object index", object_index)
object_type = cur.read_s32le()
print("object depth", object_type) # Room = 0x2
length = cur.read_s32le()
name = cur.read(length) # Room
print("name", name)

parse_generic_object()
for _ in range(3):
    object_index = cur.read_s32le()
    object_type = cur.read_s32le()
    parse_generic_object()

junk = cur.read(2384) # junk???
for _ in range(596):
    assert junk[_ * 4 : (_ + 1) * 4] == b'\xfe\xff\xff\xff'

for _ in range(1217):
    object_index = cur.read_s32le()
    object_type = cur.read_s32le()
    parse_generic_object()
    
junk = cur.read(4888)
print("junk", junk)

object_index = cur.read_s32le()
print("object index", object_index)
object_type = cur.read_s32le()
print("object depth", object_type) # MetaRoom = 0x5
length = cur.read_s32le()
name = cur.read(length) # MetaRoom
print("name", name)

parse_generic_object()

junk = cur.read(36) # all -2s

for _ in range(1, 10):
    print("metaroom", _)
    object_index = cur.read_s32le()
    object_type = cur.read_s32le()
    parse_generic_object()

junk = b''
# while b'OBST' not in cur.peek(5000):
#     junk += cur.read(5000)
junk += cur.read(288092)
print("junk", len(junk))
if b'\x89\x5a\x08\x4a\x86\x39\x46\x31\x8b\x49\x08\x32\x08\x32\xc4\x10' in junk:
    print("found it!") # hmm. not NornMeso/BLKs then...
    

object_index = cur.read_s32le()
print("object index", object_index)
object_type = cur.read_s32le()
print("object type", object_type) # SkeletalCreature = 0x6
length = cur.read_s32le()
name = cur.read(length) # SkeletalCreature
print("name", name)

parse_generic_object()

    
print(cur.peek(10000))
    
exit()

# print("creaturesarchive tag", cur.read_s32le())


"""

MacroScript -
OBST
u32 0 - ???
u32 family
u32 genus
u32 species
u32 event_number
u32 unknown - 0 - EMIT's command group? or something random?
u32 unknown - 14 - ???
u32 unknown - 107 - EMIT's command index?
u32 unknown - 1 - first argument, a raw integer. how does it know?
u16 unknown - 3 - ? third caos command group? (_P1_ is in group 2, which is the third one...)
u16 unknown - 303 - _P1_'s command index!
u16 unknown - 17 - ???
u32 object index? - 3
u32 object depth? - 1
u32 object magic number four? - 4
OBST
u32 string length
string script text
u32 unknown - 1
u32 unknown - 0
u32 unknown - 0
u32 unknown - 4 - end of object marker?
OBEN
u32 unknown - 4 - end of object marker?
OBEN
"""

# s = (
# b"OBST\x00\x00\x00\x00\x01\x00\x00\x00\x01\x00\x00\x00\x96\x00\x00\x00\xe8\x03\x00\x00\x00\x00\x00\x00\x0e\x00\x00\x00k\x00\x00\x00\x01\x00\x00\x00\x03\x00/\x01\x11\x00\x03\x00\x00\x00\x01\x00\x00\x00\x04\x00\x00\x00"
#  + b'OBST\x0c\x00\x00\x00emit 1 _p1_ \x01\x00\x00\x00\x00\x00\x00\x00\x00\x00\x00\x00\x04\x00\x00\x00OBEN'
#  + b'\x04\x00\x00\x00OBEN'
#  )
# 
# print(len(s))
# 
# print(s[:4]) # OBST
# s = s[4:]
# 
# for _ in range(9):
#     print(s[:4], struct.unpack("<i", s[:4])[0])
#     s = s[4:]
# 
# for _ in range(9):
#     print(s[:2], struct.unpack("<h", s[:2])[0])
#     s = s[2:]
# 
# print(s[:4]) # OBST
# s = s[4:]
# 
# print(s[:4], struct.unpack("<i", s[:4])[0])
# length = struct.unpack("<i", s[:4])[0]
# s = s[4:]
# 
# script = s[:length]
# print(script)
# s = s[length:]
# 
# for _ in range(4):
#     print(s[:4], struct.unpack("<i", s[:4])[0])
#     s = s[4:]
# 
# print(s[:4]) # OBEN
# s = s[4:]
# 
# for _ in range(1):
#     print(s[:4], struct.unpack("<i", s[:4])[0])
#     s = s[4:]
# 
# print(s[:4]) # OBEN
# s = s[4:]

# exit()



with open("SpaceAndAllThatIsOutThere.out", "rb") as f:
    # data = f.read()
    
    indent = 0
    buf = b''
    while True:
        # print(f.peek(4))
        if f.peek(4)[:4] == b'OBST':
            print("    " * indent, buf)
            buf = b''
            indent += 1
            buf += f.read(4)
            continue
        elif f.peek(4)[:4] == b'OBEN':
            buf += f.read(4)
            print("    " * indent, buf)
            buf = b''
            indent -= 1
            continue
        buf += f.read(1)
        # print(buf)

    # print("  " * indent, buf)
    
"""    
toplevel, repeated pattern of
\x02\x00\x00\x00 \x00\x00\x00\x00 \x04\x00\x00\x00 OBST
\x04\x00\x00\x00 \x00\x00\x00\x00 \x04\x00\x00\x00 OBST
\x06\x00\x00\x00 \x00\x00\x00\x00 \x04\x00\x00\x00 OBST
\x08\x00\x00\x00 \x00\x00\x00\x00 \x04\x00\x00\x00 OBST
\x0a\x00\x00\x00 \x00\x00\x00\x00 \x04\x00\x00\x00 OBST
\x0c\x00\x00\x00 \x00\x00\x00\x00 \x04\x00\x00\x00 OBST
between objects
 
and an internal pattern of
\x01\x00\x00\x00 \x01\x00\x00\x00 \x04\x00\x00\x00 OBST
\x03\x00\x00\x00 \x01\x00\x00\x00 \x04\x00\x00\x00 OBST

etc..
is it counting total objects, and then nesting level? what is the \x04 for?

also, the first time it happens, we have

\x00\x00\x00\x00 \x0b\0x00\x00\x00 (length of string) MacroScript \x04\x00\x00\x00 OBST

and then inside we have 

\x01\x00\x00\x00 \t\x00\x00\x00 (length of string) DebugInfo \x04\x00\x00\x00 OBST

hmm.

"""