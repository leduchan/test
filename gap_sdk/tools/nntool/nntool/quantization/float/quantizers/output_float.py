# Copyright (C) 2020  GreenWaves Technologies, SAS

# This program is free software: you can redistribute it and/or modify
# it under the terms of the GNU Affero General Public License as
# published by the Free Software Foundation, either version 3 of the
# License, or (at your option) any later version.

# This program is distributed in the hope that it will be useful,
# but WITHOUT ANY WARRANTY; without even the implied warranty of
# MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
# GNU Affero General Public License for more details.

# You should have received a copy of the GNU Affero General Public License
# along with this program.  If not, see <https://www.gnu.org/licenses/>.


from copy import deepcopy

import numpy as np
from bfloat16 import bfloat16
from nntool.graph.types import OutputNode
from nntool.quantization.float.float_quantization_handler import \
    FloatQuantizionHandler
from nntool.quantization.new_qrec import QRec
from nntool.quantization.qtype import QType
from nntool.quantization.qtype_constraint import MatchAll
from nntool.quantization.quantizer_options import QTYPE_IND_OPTION
from nntool.quantization.unified_quantization_handler import (in_qs_constraint,
                                                       options,
                                                       out_qs_constraint,
                                                       params_type)


@params_type(OutputNode)
@in_qs_constraint(MatchAll({'dtype': set([np.float32, np.float16, bfloat16])}))
@out_qs_constraint(MatchAll({'dtype': set([np.float32, np.float16, bfloat16])}))
@options(QTYPE_IND_OPTION)
class FloatOutput(FloatQuantizionHandler):
    @classmethod
    def _quantize(cls, params, in_qs, stats, **kwargs):
        force_out_qs, dtype = cls.get_float_opts(**kwargs)
        if force_out_qs and any(qtype.dtype != dtype for qtype in force_out_qs if qtype is not None):
            return None
        opts = kwargs['opts']
        o_q_ind = opts.get('qtype_ind')
        if o_q_ind:
            o_q = deepcopy(o_q_ind)
        else:
            min_val, max_val = cls.get_min_max(stats, direction='in')
            o_q = QType(dtype=dtype, min_val=min_val, max_val=max_val)
        return QRec.float(in_qs=[o_q],
                          out_qs=[o_q],
                          float_dtype=o_q.dtype)
