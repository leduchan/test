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

from cmd2 import Cmd, Cmd2ArgumentParser, with_argparser
from nntool.interpreter.nntool_shell_base import NNToolShellBase, no_history
from nntool.interpreter.shell_utils import table_options, output_table
from nntool.stats.temps_stats_collector import TempsStatsCollector
from nntool.reports.temps_reporter import TempsReporter

class TempsCommand(NNToolShellBase):
    # TEMPS COMMAND
    parser_temps = Cmd2ArgumentParser()
    table_options(parser_temps, default_width=140)

    @with_argparser(parser_temps)
    @no_history
    def do_temps(self, args):
        """
Show statistics on activations."""
        self._check_graph()
        fmt = ('tab' if args.output is None else args.output['fmt'])
        stats_collector = TempsStatsCollector()
        stats = stats_collector.collect_stats(self.G)
        tab = TempsReporter(do_totals=(fmt != "csv")).report(self.G, stats)
        output_table(tab, args)
