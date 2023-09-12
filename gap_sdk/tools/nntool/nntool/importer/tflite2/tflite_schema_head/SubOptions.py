# automatically generated by the FlatBuffers compiler, do not modify

# namespace: tflite_schema_head

import flatbuffers
from flatbuffers.compat import import_numpy
np = import_numpy()

class SubOptions(object):
    __slots__ = ['_tab']

    @classmethod
    def GetRootAs(cls, buf, offset=0):
        n = flatbuffers.encode.Get(flatbuffers.packer.uoffset, buf, offset)
        x = SubOptions()
        x.Init(buf, n + offset)
        return x

    @classmethod
    def GetRootAsSubOptions(cls, buf, offset=0):
        """This method is deprecated. Please switch to GetRootAs."""
        return cls.GetRootAs(buf, offset)
    @classmethod
    def SubOptionsBufferHasIdentifier(cls, buf, offset, size_prefixed=False):
        return flatbuffers.util.BufferHasIdentifier(buf, offset, b"\x54\x46\x4C\x33", size_prefixed=size_prefixed)

    # SubOptions
    def Init(self, buf, pos):
        self._tab = flatbuffers.table.Table(buf, pos)

    # SubOptions
    def FusedActivationFunction(self):
        o = flatbuffers.number_types.UOffsetTFlags.py_type(self._tab.Offset(4))
        if o != 0:
            return self._tab.Get(flatbuffers.number_types.Int8Flags, o + self._tab.Pos)
        return 0

    # SubOptions
    def PotScaleInt16(self):
        o = flatbuffers.number_types.UOffsetTFlags.py_type(self._tab.Offset(6))
        if o != 0:
            return bool(self._tab.Get(flatbuffers.number_types.BoolFlags, o + self._tab.Pos))
        return True

def SubOptionsStart(builder): builder.StartObject(2)
def Start(builder):
    return SubOptionsStart(builder)
def SubOptionsAddFusedActivationFunction(builder, fusedActivationFunction): builder.PrependInt8Slot(0, fusedActivationFunction, 0)
def AddFusedActivationFunction(builder, fusedActivationFunction):
    return SubOptionsAddFusedActivationFunction(builder, fusedActivationFunction)
def SubOptionsAddPotScaleInt16(builder, potScaleInt16): builder.PrependBoolSlot(1, potScaleInt16, 1)
def AddPotScaleInt16(builder, potScaleInt16):
    return SubOptionsAddPotScaleInt16(builder, potScaleInt16)
def SubOptionsEnd(builder): return builder.EndObject()
def End(builder):
    return SubOptionsEnd(builder)