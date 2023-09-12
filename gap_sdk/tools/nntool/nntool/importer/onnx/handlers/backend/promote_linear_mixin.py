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

import numpy as np
from nntool.importer.common.constant_mixin import ConstantMixin


class PromoteLinearMixin(ConstantMixin):
    @classmethod
    def is_linear(cls, y, x_shape, y_shape):
        return (((len(x_shape) == 1 and len(y_shape) == 2 and y_shape[0] == x_shape[0]) or \
                 (len(x_shape) == 2 and any(x == 1 for x in x_shape) and len(y_shape) == 2 and y_shape[0] == np.prod(x_shape))) \
                and cls.is_constant(y))
