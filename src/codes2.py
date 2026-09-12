#    File:     codes2.py
#    Version:  1.2.0
#    Date:     July 24, 2026
#    Author:   Michal Babik <michal.babik@protonmail.com>
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
from pcodespl import sql_get_city_names_like, sql_get_city_voivodeship, \
                     sql_get_info
#-----------------------------------------------------------------------------#
class AppWindow(Gtk.Window):
    def __init__(self):
        Gtk.Window.__init__(self)
        self.set_title("PostalCodesPL")
        self.set_position(Gtk.WindowPosition.CENTER)
        self.set_default_size(800, 600)
        self.pre_len = 0
        self.ching = False
        self.ct_id = 0
        self.voiv_id = 0
        vbox1 = Gtk.Box(orientation=Gtk.Orientation.VERTICAL, spacing=0)
        grid = Gtk.Grid()
        grid.set_orientation(Gtk.Orientation.HORIZONTAL)
        grid.set_column_spacing(8)
        grid.set_margin_start(8)
        grid.set_margin_end(8)

        lab = Gtk.Label()
        lab.set_markup("<b><i>Enter and select Place name, "
                       "Voivodeship and Street</i></b>")
        vbox1.pack_start(lab, False, True, 8)

        lab = Gtk.Label()
        lab.set_label("Place:")
        grid.add(lab)

        city_store = Gtk.ListStore(int, str)
        self.city_combo = Gtk.ComboBox.new_with_model_and_entry(city_store)
        self.city_combo.connect("changed", self.on_city_combo_changed)
        self.city_combo.set_entry_text_column(1)
        self.city_combo.set_tooltip_text(
                "After entering 3 letters list of pollible Places updates")
        grid.add(self.city_combo)

        lab = Gtk.Label()
        lab.set_label("Voivodeship:")
        grid.add(lab)

        self.voivo_store = Gtk.ListStore(int, str)
        self.voivo_combo = Gtk.ComboBox.new_with_model(self.voivo_store)
        renderer_text = Gtk.CellRendererText()
        self.voivo_combo.pack_start(renderer_text, True)
        self.voivo_combo.add_attribute(renderer_text, "text", 1)
        self.voivo_combo.connect("changed", self.on_voivo_combo_changed)
        self.voivo_combo.set_entry_text_column(1)
        grid.add(self.voivo_combo)

        lab = Gtk.Label()
        lab.set_label("Street:")
        grid.add(lab)

        self.street_entry = Gtk.Entry()
        self.street_entry.connect("changed", self.street_entry_changed)
        self.street_entry.set_tooltip_text(
                "After entering 3 letters postal code info will be filtered")
        grid.add(self.street_entry)
        
        vbox1.pack_start(grid, False, True, 8)
        self.model = Gtk.ListStore(str, str, str, str, str, str)
        self.treev = Gtk.TreeView.new_with_model(self.model)
        for i, n in enumerate(["Post code", "Place", "Place det.",
                               "Street", "Number", "Post unit"]):
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
    def on_city_combo_changed(self, combo):
        if self.ching:
            return
        self.ching = True
        try:
            itr = combo.get_active_iter()
            if itr is not None:
                model = combo.get_model()
                row_id, name = model[itr][:2]
                self.ct_id = row_id
                r, d = sql_get_city_voivodeship(row_id)
                self.voivo_store.clear()
                if r:
                    voiv_di = {}
                    for i in d:
                        if i[0] not in voiv_di:
                            self.voivo_store.append((i[0], i[1],))
                            voiv_di[i[0]] = 0
                    if len(self.voivo_store) > 0:
                        self.voivo_combo.set_active(-1)
                        self.voivo_combo.set_active(0)
            else:
                entry = combo.get_child()
                en_txt = entry.get_text()
                model = combo.get_model()
                if len(en_txt) > 2:
                    model.clear()
                    r, d = sql_get_city_names_like(en_txt)
                    if not r: 
                        self.ching = False
                        return
                    for i in d:
                        model.append(i)
                else:
                    model.clear()
        finally:
            self.ching = False
    #-------------------------------------------------------------------------#
    def on_voivo_combo_changed(self, combo):
        itr = combo.get_active_iter()
        if itr is not None:
            model = combo.get_model()
            row_id, name = model[itr][:2]
            self.voiv_id = row_id
            self.refresh_data_list()
    #-------------------------------------------------------------------------#
    def street_entry_changed(self, entry):
        en_len = len(entry.get_text())
        if en_len > 2 or self.pre_len > 2:
            self.refresh_data_list()
        self.pre_len = en_len
    #-------------------------------------------------------------------------#
    def refresh_data_list(self):
        if self.ct_id > 0 and self.voiv_id > 0:
            en_txt = self.street_entry.get_text()
            to_sql_street = ''
            if len(en_txt) > 2:
                to_sql_street = en_txt
            r, d = sql_get_info(self.ct_id, self.voiv_id, to_sql_street)
            if r:
                self.model.clear()
                for i in d:
                    self.model.append(
                          (i[0] + '-' + i[1], i[2], i[3], i[4], i[5], i[6], ))
#-----------------------------------------------------------------------------#
if __name__ == "__main__":
    win = AppWindow()
    win.show_all()
    Gtk.main()
#-----------------------------------------------------------------------------#

