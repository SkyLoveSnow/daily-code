# target actual expected
# Case: actual is Hash
#   when expected is Hash and actual includes the expected Hash at least partially, returns true
#     e.g) actual: {:foo => :bar, :hoge => :fuga} and expected: {:hoge => :fuga} #=> true
#   but when expected value is included as child value, returns false
#     e.g) actual: {:foo => :bar, :hoge => {:fuga  => :piyo}} and expected: {:fuga  => :piyo} #=> false
# Case: actual is Array
#   when expected is Array and actual includes the expected at least partially, returns true
#     e.g) actual: [1,2,3] and expected: [1,2] #=> true
#   when expected is not Array and actual includes the expected value as item, returns true
#     e.g) actual: [1,2,3] and expected: 1 #=> true
# In above cases, if child value is Array of Hash, same match logic is applied recursively.
#
# Other cases:
#   when expected equals to actual returns true
#     e.g) actual: 1 and expected: 1 #=> true
import json
import types
import copy
import functools

class JSONParseException(Exception):
    def __init__(self,arg):
        super(JSONParseException,self).__init__(self,"Json parse error:{}".format(json.dumps(arg)))

def json_parse(arg):
    if type(arg) == types.StringType:
        return json.loads(arg)
    elif type(arg) != types.DictType and type(arg) != types.ListType:
         raise JSONParseException(arg)
    else:
        return arg

def hash_partial_include(actual_hash, expected_hash):
    if type(actual_hash) != types.DictType or type(expected_hash) != types.DictType:
        return False
    results = []
    for key,value in expected_hash.iteritems():
        if actual_hash.has_key(key):
            if type(value) == types.DictType:
                result = hash_partial_include(actual_hash[key],value)
            elif type(value) == types.ListType:
                result = all(map(functools.partial(array_partial_include,actual_hash[key]),value))
            else:
                result = (value == actual_hash[key])
        else:
            return False
        results.append(result)
    return all(results)

def array_partial_include(actual_array, expected_item):
    results = []
    for actual_item in actual_array:
        if type(actual_item) == types.DictType:
            result = (type(expected_item) == types.DictType and hash_partial_include(actual_item,expected_item))
        elif type(actual_item) == types.ListType:
            result = (type(expected_item) == types.ListType and all(map(functools.partial(array_partial_include,actual_item),expected_item)))
        else:
            result = (actual_item == expected_item)
        results.append(result)
    return any(results)

def include_json(actual,expected):
    actual = json_parse(actual)
    expected = json_parse(expected)
    if type(actual) == types.ListType:
        if type(expected) == types.ListType:
            return all(map(functools.partial(array_partial_include,actual),expected))
        array_partial_include(actual,expected)
    elif type(actual) == types.DictType:
        return hash_partial_include(actual,expected)
    else:
        return actual == expected


import testtools
class TestIncludeJson(testtools.TestCase):
    test_hash = {
        "name":"test",
        "status":"ok",
        "nested":{
            "memory":"1mb",
            "os":"win7"
        }
    }

    test_array = ["aaa","bbb",1,5]

    def test_array_array1(self):
        self.assertTrue(include_json([1,3],[1]))

    def test_array_array2(self):
        self.assertFalse(include_json([1,3],[2]))

    def test_array_nested1(self):
        self.assertFalse(include_json([1,3,[2,4]],[2,4]))

    def test_array_nested2(self):
        self.assertTrue(include_json([1,3,[2,4]],[[2,4]]))

    def test_hash_hash1(self):
        self.assertTrue(include_json(self.test_hash,{"name":"test"}))

    def test_hash_hash2(self):
        self.assertFalse(include_json(self.test_hash,{"name":"test_error"}))

    def test_hash_hash3(self):
        self.assertTrue(include_json(self.test_hash,{"nested":self.test_hash["nested"]}))

    def test_hash_hash4(self):
        self.assertFalse(include_json(self.test_hash,self.test_hash["nested"]))

    def test_array_hash1(self):
        array = copy.deepcopy(self.test_array)
        array.append(self.test_hash)
        self.assertTrue(include_json(array,["aaa",self.test_hash]))

    def test_array_hash2(self):
        array = copy.deepcopy(self.test_array)
        array.append(self.test_hash)
        self.assertFalse(include_json(array,["aaa",self.test_hash["nested"]]))

    def test_array_hash3(self):
        array = copy.deepcopy(self.test_array)
        array.append(self.test_hash)
        self.assertFalse(include_json(array,[1,"aaa",{"not_exist":"test"}]))

    def test_hash_array1(self):
        hash = copy.deepcopy(self.test_hash)
        hash["array"] = self.test_array
        array = copy.deepcopy(self.test_array)
        array.pop()
        self.assertTrue(include_json(hash,{"array":array}))
        self.assertTrue(include_json(hash,{"nested":{"os":self.test_hash["nested"]["os"]}}))
        array.append("dast")
        self.assertFalse(include_json(hash,{"nested":{"os":self.test_hash["nested"]["os"]},"array":array}))