#    File:     codes1.py
#    Version:  1.2
#    Date:     July 24, 2026
#    Author:   Michal Babik <michalb1981@o2.pl>
#    Copyright (C) 2016-2026 Michal Babik
#
#    This program is free software: you can redistribute it and/or modify
#    it under the terms of the GNU General Public License as published by
#    the Free Software Foundation, either version 3 of the License, or
#    (at your option) any later version.
#
#    This program is distributed in the hope that it will be useful,
#    but WITHOUT ANY WARRANTY; without even the implied warranty of
#    MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
#    GNU General Public License for more details.
#
#    You should have received a copy of the GNU General Public License
#    along with this program.  If not, see <http://www.gnu.org/licenses/>.
#-----------------------------------------------------------------------------#
import gi
gi.require_version('Gtk', '3.0')
from gi.repository import Gtk
from pcodespl import sql_get_post_code_info
#-----------------------------------------------------------------------------#
class AppWindow(Gtk.Window):
    def __init__(self):
        Gtk.Window.__init__(self)
        self.set_title("PostalCodesPL")
        self.set_position(Gtk.WindowPosition.CENTER)
        self.set_default_size(800, 600)
        vbox1 = Gtk.Box(orientation=Gtk.Orientation.VERTICAL, spacing=0)
        # hbox1 = Gtk.Box(orientation=Gtk.Orientation.HORIZONTAL, spacing=0)
        grid = Gtk.Grid()
        grid.set_orientation(Gtk.Orientation.HORIZONTAL)
        grid.set_column_spacing(8)
        grid.set_margin_start(8)
        grid.set_margin_end(8)
        self.en1 = Gtk.Entry()
        self.en1.set_max_length(2)
        self.en1.set_width_chars(2)
        self.en1.set_max_width_chars(2)
        self.en1.connect("changed", self.entry_ch, 2)
        grid.add(self.en1)
        lab = Gtk.Label()
        lab.set_label("-")
        grid.add(lab)
        en2 = Gtk.Entry()
        en2.set_max_length(3)
        en2.set_width_chars(3)
        en2.set_max_width_chars(3)
        en2.connect("changed", self.entry_ch, 3)
        grid.add(en2)
        lab = Gtk.Label()
        lab.set_markup("<b><i>Enter the zip code for which the data should be"
                       " displayed</i></b>")
        grid.add(lab)
        vbox1.pack_start(grid, False, True, 8)
        self.model = Gtk.ListStore(str, str, str, str, str, str)
        self.treev = Gtk.TreeView.new_with_model(self.model)
        for i, n in enumerate(["Place", "Place det.", "Street", 
                               "Number", "Postal unit", "Voivodeship"]):
            rendererText = Gtk.CellRendererText()
            column = Gtk.TreeViewColumn(n, rendererText, text = i)
            column.set_resizable(True)
            self.treev.append_column(column)
        scrl = Gtk.ScrolledWindow()
        scrl.set_policy(Gtk.PolicyType.AUTOMATIC, Gtk.PolicyType.AUTOMATIC)
        scrl.add(self.treev)
        vbox1.pack_start(scrl, True, True, 8)
        self.add(vbox1)
        self.connect("delete_event", Gtk.main_quit)
        self.connect("destroy", Gtk.main_quit)
    #-------------------------------------------------------------------------#
    def entry_ch(self, widget, leng):
        if len(widget.get_text()) == leng:
            if leng == 3:
                tx2 = widget.get_text()
                tx1 = self.en1.get_text()
                r, d = sql_get_post_code_info(tx1, tx2)
                if r:
                    self.model.clear()
                    for i in d: self.model.append(i)
            self.get_ancestor(Gtk.Window).do_move_focus(
                self.get_ancestor(Gtk.Window), Gtk.DirectionType.TAB_FORWARD)
        return
#-----------------------------------------------------------------------------#
def infodial(widget, t):
    md = Gtk.MessageDialog(widget,
                           Gtk.DialogFlags.MODAL | 
                           Gtk.DialogFlags.DESTROY_WITH_PARENT,
                           Gtk.MessageType.INFO,
                           Gtk.ButtonsType.OK,
                           t)
    md.set_title('Information')
    md.run()
    md.destroy()
#-----------------------------------------------------------------------------#
if __name__ == "__main__":
    win = AppWindow()
    win.show_all()
    Gtk.main()
#-----------------------------------------------------------------------------#

