import gdb
import gdb.printing
import printer

#gdb.write("Loading Novembre printers...\n")

gdb.printing.register_pretty_printer(
	gdb.current_objfile(),
	printer.deref_printer)

gdb.printing.register_pretty_printer(
	gdb.current_objfile(),
	printer.build_pretty_printer())
