# Copyright (c) 2016-2026 Michal Babik
# SPDX-License-Identifier: GPL-3.0-or-later
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
        self.code_entry_first = Gtk.Entry()
        self.code_entry_first.set_max_length(2)
        self.code_entry_first.set_width_chars(2)
        self.code_entry_first.set_max_width_chars(2)
        self.code_entry_first.connect("changed", self.entry_changed, 2)
        grid.add(self.code_entry_first)
        lab = Gtk.Label()
        lab.set_label("-")
        grid.add(lab)
        code_entry_second = Gtk.Entry()
        code_entry_second.set_max_length(3)
        code_entry_second.set_width_chars(3)
        code_entry_second.set_max_width_chars(3)
        code_entry_second.connect("changed", self.entry_changed, 3)
        grid.add(code_entry_second)
        lab = Gtk.Label()
        lab.set_markup("<b><i>Enter the zip code for which the data should be"
                       " displayed</i></b>")
        grid.add(lab)
        vbox1.pack_start(grid, False, True, 8)
        self.model = Gtk.ListStore(str, str, str, str, str, str)
        self.tree_view = Gtk.TreeView.new_with_model(self.model)
        for i, n in enumerate(["Place", "Place det.", "Street", 
                               "Number", "Postal unit", "Voivodeship"]):
            rendererText = Gtk.CellRendererText()
            column = Gtk.TreeViewColumn(n, rendererText, text = i)
            column.set_resizable(True)
            self.tree_view.append_column(column)
        scrolled = Gtk.ScrolledWindow()
        scrolled.set_policy(Gtk.PolicyType.AUTOMATIC, Gtk.PolicyType.AUTOMATIC)
        scrolled.add(self.tree_view)
        vbox1.pack_start(scrolled, True, True, 8)
        self.add(vbox1)
        self.connect("delete_event", Gtk.main_quit)
        self.connect("destroy", Gtk.main_quit)
    #-------------------------------------------------------------------------#
    def entry_changed(self, widget, length: int):
        if len(widget.get_text()) == length:
            if length == 3:
                tx2 = widget.get_text()
                tx1 = self.code_entry_first.get_text()
                r, d = sql_get_post_code_info(tx1, tx2)
                if r:
                    self.model.clear()
                    for i in d:
                        self.model.append(i)
                else:
                    error_dialog(self, d)
            self.get_ancestor(Gtk.Window).do_move_focus(
                self.get_ancestor(Gtk.Window), Gtk.DirectionType.TAB_FORWARD)
#-----------------------------------------------------------------------------#
def error_dialog(widget, t):
    md = Gtk.MessageDialog(widget,
                           Gtk.DialogFlags.MODAL | 
                           Gtk.DialogFlags.DESTROY_WITH_PARENT,
                           Gtk.MessageType.ERROR,
                           Gtk.ButtonsType.OK,
                           t)
    md.set_title('Error')
    md.run()
    md.destroy()
#-----------------------------------------------------------------------------#
if __name__ == "__main__":
    win = AppWindow()
    win.show_all()
    Gtk.main()
#-----------------------------------------------------------------------------#

