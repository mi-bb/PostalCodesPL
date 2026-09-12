# Copyright (c) 2016-2026 Michal Babik
# SPDX-License-Identifier: GPL-3.0-or-later
#-----------------------------------------------------------------------------#
import gi
gi.require_version('Gtk', '3.0')
from gi.repository import Gtk
from pcodespl import (sql_get_city_names_like, sql_get_city_voivodeship,
                      sql_get_info)
#-----------------------------------------------------------------------------#
class AppWindow(Gtk.Window):
    def __init__(self):
        Gtk.Window.__init__(self)
        self.set_title("PostalCodesPL")
        self.set_position(Gtk.WindowPosition.CENTER)
        self.set_default_size(800, 600)
        self.pre_len: int = 0
        self.changing: bool = False
        self.city_id: int = 0
        self.voivodeship_id: int = 0
        vbox1 = Gtk.Box(orientation=Gtk.Orientation.VERTICAL, spacing=0)
        grid = Gtk.Grid()
        grid.set_orientation(Gtk.Orientation.HORIZONTAL)
        grid.set_column_spacing(8)
        grid.set_margin_start(8)
        grid.set_margin_end(8)

        title_label = Gtk.Label()
        title_label.set_markup("<b><i>Enter and select Place name, "
                       "Voivodeship and Street</i></b>")
        vbox1.pack_start(title_label, False, True, 8)

        place_label = Gtk.Label()
        place_label.set_label("Place:")
        grid.add(place_label)

        city_store = Gtk.ListStore(int, str)
        self.city_combo = Gtk.ComboBox.new_with_model_and_entry(city_store)
        self.city_combo.connect("changed", self.on_city_combo_changed)
        self.city_combo.set_entry_text_column(1)
        self.city_combo.set_tooltip_text(
                "After entering 3 letters list of pollible Places updates")
        grid.add(self.city_combo)

        voivodeship_label = Gtk.Label()
        voivodeship_label.set_label("Voivodeship:")
        grid.add(voivodeship_label)

        self.voivodeship_list_store = Gtk.ListStore(int, str)
        self.voivodeship_combo = Gtk.ComboBox.new_with_model(
            self.voivodeship_list_store)
        combo_renderer_text = Gtk.CellRendererText()
        self.voivodeship_combo.pack_start(combo_renderer_text, True)
        self.voivodeship_combo.add_attribute(combo_renderer_text, "text", 1)
        self.voivodeship_combo.connect(
            "changed",self.voivodeship_combo_changed)
        self.voivodeship_combo.set_entry_text_column(1)
        grid.add(self.voivodeship_combo)

        street_label = Gtk.Label()
        street_label.set_label("Street:")
        grid.add(street_label)

        self.street_entry = Gtk.Entry()
        self.street_entry.connect("changed", self.street_entry_changed)
        self.street_entry.set_tooltip_text(
                "After entering 3 letters postal code info will be filtered")
        grid.add(self.street_entry)
        
        vbox1.pack_start(grid, False, True, 8)
        self.model = Gtk.ListStore(str, str, str, str, str, str)
        self.tree_view = Gtk.TreeView.new_with_model(self.model)
        for i, n in enumerate(["Post code", "Place", "Place det.",
                               "Street", "Number", "Post unit"]):
            tree_renderer_text = Gtk.CellRendererText()
            column = Gtk.TreeViewColumn(n, tree_renderer_text, text = i)
            column.set_resizable(True)
            self.tree_view.append_column(column)
        scrolled_window = Gtk.ScrolledWindow()
        scrolled_window.set_policy(
            Gtk.PolicyType.AUTOMATIC, Gtk.PolicyType.AUTOMATIC)
        scrolled_window.add(self.tree_view)
        vbox1.pack_start(scrolled_window, True, True, 8)
        self.add(vbox1)
        self.connect("delete_event", Gtk.main_quit)
        self.connect("destroy", Gtk.main_quit)
    #-------------------------------------------------------------------------#
    def on_city_combo_changed(self, combo):
        if self.changing:
            return
        self.changing = True
        try:
            itr = combo.get_active_iter()
            if itr is not None:
                model = combo.get_model()
                row_id, _name = model[itr][:2]
                self.city_id = row_id
                r, d = sql_get_city_voivodeship(row_id)
                self.voivodeship_list_store.clear()
                if r:
                    voivodeship_dict = {}
                    for i in d:
                        if i[0] not in voivodeship_dict:
                            self.voivodeship_list_store.append((i[0], i[1],))
                            voivodeship_dict[i[0]] = 0
                    if len(self.voivodeship_list_store) > 0:
                        self.voivodeship_combo.set_active(-1)
                        self.voivodeship_combo.set_active(0)
            else:
                entry = combo.get_child()
                entry_txt = entry.get_text()
                model = combo.get_model()
                if len(entry_txt) > 2:
                    model.clear()
                    r, d = sql_get_city_names_like(entry_txt)
                    if not r: 
                        self.changing = False
                        return
                    for i in d:
                        model.append(i)
                else:
                    model.clear()
        finally:
            self.changing = False
    #-------------------------------------------------------------------------#
    def voivodeship_combo_changed(self, combo):
        itr = combo.get_active_iter()
        if itr is not None:
            model = combo.get_model()
            row_id, _name = model[itr][:2]
            self.voivodeship_id = row_id
            self.refresh_data_list()
    #-------------------------------------------------------------------------#
    def street_entry_changed(self, entry):
        entry_len = len(entry.get_text())
        if entry_len > 2 or self.pre_len > 2:
            self.refresh_data_list()
        self.pre_len = entry_len
    #-------------------------------------------------------------------------#
    def refresh_data_list(self):
        if self.city_id > 0 and self.voivodeship_id > 0:
            entry_txt = self.street_entry.get_text()
            to_sql_street = ''
            if len(entry_txt) > 2:
                to_sql_street = entry_txt
            r, d = sql_get_info(
                self.city_id, self.voivodeship_id, to_sql_street)
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

