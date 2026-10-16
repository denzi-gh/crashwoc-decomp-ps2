"""Unit tests for the GCC 2.9 STABS type parser (tools/declib/stabs.py).

The stab strings are written in the shape GCC 2.9-ee-991111 emits for the
retail `sce` units; no game files are needed.
"""
import sys
import unittest
from pathlib import Path

ROOT = Path(__file__).resolve().parent.parent
sys.path.insert(0, str(ROOT / "tools"))
from declib.stabs import StabsError, TypeTable  # noqa: E402

BASIC = [
    "int:t1=r1;-2147483648;2147483647;",
    "char:t2=r2;0;127;",
    "long int:t3=r1;001000000000000000000000;000777777777777777777777;",
    "unsigned int:t4=r1;0;-1;",
    "long unsigned int:t5=r1;000000000000000000000000;001777777777777777777777;",
    "unsigned char:t11=r11;0;255;",
    "float:t12=r1;4;0;",
    "double:t13=r1;8;0;",
    "void:t19=19",
]


def table(*strings):
    t = TypeTable()
    for s in BASIC + list(strings):
        t.add(s)
    return t


def typed(t, string):
    _name, desc, ref = t.add(string)
    return desc, t.c_type(ref), t.size(ref)


class TestBasicTypes(unittest.TestCase):
    def test_sizes_from_ranges(self):
        t = table()
        for string, size in (("i:G1", 4), ("c:G2", 1), ("l:G3", 8),
                             ("u:G4", 4), ("ul:G5", 8), ("b:G11", 1),
                             ("f:G12", 4), ("d:G13", 8)):
            self.assertEqual(typed(t, string)[2], size, string)

    def test_typedef_names_spell_the_type(self):
        t = table()
        self.assertEqual(typed(t, "one:S13"), ("S", "double", 8))
        self.assertEqual(typed(t, "n:G5"), ("G", "long unsigned int", 8))

    def test_void_is_a_self_reference(self):
        t = table()
        self.assertEqual(typed(t, "p:G20=*19"), ("G", "void *", 4))


class TestDerivedTypes(unittest.TestCase):
    def test_open_array_has_no_size(self):
        t = table()
        self.assertEqual(typed(t, "bp:S80=ar1;0;-1;13"), ("S", "double[]", None))

    def test_complete_array(self):
        t = table()
        self.assertEqual(typed(t, "TWO52:S81=ar1;0;1;13"), ("S", "double[2]", 16))
        self.assertEqual(typed(t, "blanks:V82=ar1;0;15;2"), ("V", "char[16]", 16))

    def test_self_referencing_struct(self):
        t = table("node:T20=s8next:21=*20,0,32;val:1,32,32;;")
        self.assertEqual(typed(t, "head:G20"), ("G", "struct node", 8))
        self.assertEqual(typed(t, "ptr:G21"), ("G", "struct node *", 4))

    def test_union_and_enum(self):
        t = table("__dmath:T22=u8d:13,0,64;i:23=ar1;0;1;4,0,64;;",
                  "color:T24=eRED:0,GREEN:1,;")
        self.assertEqual(typed(t, "__infinity:G22"), ("G", "union __dmath", 8))
        self.assertEqual(typed(t, "c:G24"), ("G", "enum color", 4))

    def test_function_pointer_typedef_array(self):
        t = table("func_ptr:t25=26=*27=f19")
        self.assertEqual(typed(t, "__CTOR_LIST__:G28=ar1;0;1;25"),
                         ("G", "func_ptr[2]", 8))
        self.assertEqual(typed(t, "fp:G29=*30=f1"), ("G", "int (*)()", 4))

    def test_pointer_array(self):
        t = table("mbinptr:t31=32=*2")
        self.assertEqual(typed(t, "av:G33=ar1;0;257;31"), ("G", "mbinptr[258]", 1032))

    def test_forward_reference_resolves_to_its_tag(self):
        t = table("q:G40=*41=xsfwd:", "fwd:T42=s4x:1,0,32;;")
        self.assertEqual(typed(t, "q:G40"), ("G", "struct fwd *", 4))
        self.assertEqual(t.size(t.add("v:G41")[2]), 4)

    def test_typedef_of_anonymous_struct(self):
        t = table("lconv_t:t43=s8a:1,0,32;b:1,32,32;;")
        self.assertEqual(typed(t, "lconv:S43"), ("S", "lconv_t", 8))


class TestErrors(unittest.TestCase):
    def test_unsupported_descriptor_raises(self):
        with self.assertRaises(StabsError):
            table().add("x:t50=#1,2;")

    def test_undefined_reference_raises(self):
        t = table()
        _n, _d, ref = t.add("x:G99")
        with self.assertRaises(StabsError):
            t.size(ref)

    def test_non_type_strings_pass(self):
        self.assertEqual(table().add("main:F1")[1], "F")
        self.assertEqual(table().add("file.c")[2], None)


if __name__ == "__main__":
    unittest.main()
