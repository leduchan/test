# automatically generated by the FlatBuffers compiler, do not modify

# namespace: tflite_schema_head

import flatbuffers
from flatbuffers.compat import import_numpy
np = import_numpy()

class LogicalNotOptions(object):
    __slots__ = ['_tab']

    @classmethod
    def GetRootAs(cls, buf, offset=0):
        n = flatbuffers.encode.Get(flatbuffers.packer.uoffset, buf, offset)
        x = LogicalNotOptions()
        x.Init(buf, n + offset)
        return x

    @classmethod
    def GetRootAsLogicalNotOptions(cls, buf, offset=0):
        """This method is deprecated. Please switch to GetRootAs."""
        return cls.GetRootAs(buf, offset)
    @classmethod
    def LogicalNotOptionsBufferHasIdentifier(cls, buf, offset, size_prefixed=False):
        return flatbuffers.util.BufferHasIdentifier(buf, offset, b"\x54\x46\x4C\x33", size_prefixed=size_prefixed)

    # LogicalNotOptions
    def Init(self, buf, pos):
        self._tab = flatbuffers.table.Table(buf, pos)

def LogicalNotOptionsStart(builder): builder.StartObject(0)
def Start(builder):
    return LogicalNotOptionsStart(builder)
def LogicalNotOptionsEnd(builder): return builder.EndObject()
def End(builder):
    return LogicalNotOptionsEnd(builder)