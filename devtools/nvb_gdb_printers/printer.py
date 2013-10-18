import gdb.printing
import gdb.types
import gdb
from qt4 import QListPrinter, QStringPrinter

import traceback

NVBSelectorCase_dict = {}
NVBSelectorRules_dict = {}
NVBSelectorRule_dict = {}

class ExpandablePrettyPrinter(gdb.printing.PrettyPrinter):
	def __init__(self,val):
		self.val = val
	def to_string(self):
		try:
			val = self.strval
			if isinstance(val,str):
				return val
			else:
				return gdb.default_visualizer(self.strval).to_string()
		except Exception:
			return None

class StringPrinter(gdb.printing.PrettyPrinter):
	def __init__(self,string):
		self.string = string
	
	def to_string(self):
		return self.string
	
	def display_hint(self):
		return 'string'

class StringPlusPrinter(StringPrinter):
	def __init__(self,string,val):
		self.string = string
		self.printer = gdb.default_visualizer(val)

	def to_string(self):
		return self.string.format(self.printer.to_string())

def NVBAxisSelectorPrinter(val):
	global NVBSelectorCase_dict
	if not NVBSelectorCase_dict:
		NVBSelectorCase_dict = gdb.types.make_enum_dict(gdb.lookup_type("NVBSelectorCase::Type"))
	try:
		if val["t"] == NVBSelectorCase_dict["NVBSelectorCase::OR"]:
			# Pretend I'm QList<NVBSelectorCase>
			return QListPrinter(val["cases"],"QList","NVBSelectorCase")
		elif val["t"] == NVBSelectorCase_dict["NVBSelectorCase::AND"]:
			# Pretend I'm NVBSelectorDataset
			return NVBSelectorDatasetPrinter(val["dataset"])
		else:
			return StringPrinter("Undefined case")
	except Exception:
		gdb.write(traceback.format_exc())

def NVBSelectorRulesPrinter(val):
	global NVBSelectorRules_dict
	if not NVBSelectorRules_dict:
		try:
			NVBSelectorRules_dict = gdb.types.make_enum_dict(gdb.lookup_type("NVBSelectorRules::NVBSelectorRulesType"))
		except Exception:
			return StringPrinter("Error")
	rules_type = val["p"]["type"]
	if rules_type == NVBSelectorRules_dict["NVBSelectorRules::Axis"]:
		return NVBSelectorAxisPrinter(val)
	elif rules_type == NVBSelectorRules_dict["NVBSelectorRules::Map"]:
		return NVBSelectorMapPrinter(val)
	elif rules_type == NVBSelectorRules_dict["NVBSelectorRules::Dataset"]:
		return NVBSelectorDatasetPrinter(val)
	else:
		return StringPrinter("Invalid rules type")

class NVBSelectorDatasetPrinter:
	"""Print NVBSelectorDataset"""

	def __init__(self, val):
			self.val = val["p"]
			#self.val["rules"].__dict__["_rules_type"] = self.val["type"]

	def children(self):
		return [ \
			("Rules",self.val["rules"]), \
			("Subrules",self.val["subrules"]) ]
		
	def to_string(self):
		return ""

class NVBSelectorAxisPrinter:
	"""Print NVBSelectorDataset"""

	def __init__(self, val):
			self.val = val["p"]

	def children(self):
		return [ \
			("Similar axes",self.val["more"]), \
			("Similarity by",self.val["buddyType"]), \
			("Rules",self.val["rules"]), \
			("Subrules",self.val["subrules"]) \
			]
		
	def to_string(self):
		return ""

class NVBSelectorMapPrinter:
	"""Print NVBSelectorDataset"""

	def __init__(self, val):
			self.val = val["p"]

	def children(self):
		return [ \
			("Coverage",self.val["mapCoverage"]), \
			("Rules",self.val["rules"]), \
			("Subrules",self.val["subrules"]) \
			]
		
	def to_string(self):
		return ""

class NVBSelectorRuleListPrinter:
	"Print a QList<NVBSelectorRule>"

	class _iterator:
		def __init__(self, nodetype, d, ruletype):
				self.nodetype = nodetype
				self.d = d
				self.count = 0
				self.ruletype = ruletype

		def __iter__(self):
				return self

		def next(self):
			global NVBSelectorRules_dict
			
			if self.count >= self.d['end'] - self.d['begin']:
				raise StopIteration
			count = self.count
			array = self.d['array'][self.d['begin'] + count]
			
			# OK, NVBSelectorRule is large!		
			node = array.cast(gdb.lookup_type('QList<%s>::Node' % self.nodetype).pointer())

			self.count = self.count + 1
			
			# Right, get the rule
			rule = node['v'].cast(self.nodetype)
			if rule["type"] == NVBSelectorRule_dict["NVBSelectorRules::Invalid"]:
				return ("Invalid",rule["type"])
			if rule["type"] == NVBSelectorRule_dict["NVBSelectorRules::Index"]:
				return ("Index", rule["i"])
			if rule["type"] == NVBSelectorRule_dict["NVBSelectorRules::Name"]:
				return ("Name",rule["n"])
			if rule["type"] == NVBSelectorRule_dict["NVBSelectorRules::MinSize"]:
				return ("Min size", rule["i"])
			if rule["type"] == NVBSelectorRule_dict["NVBSelectorRules::MaxSize"]:
				return ("Max size", rule["i"])
			if rule["type"] == NVBSelectorRule_dict["NVBSelectorRules::Size"]:
				return ("Size", rule["i"])
			if rule["type"] == NVBSelectorRule_dict["NVBSelectorRules::Units"]:
				return ("Units", rule["u"])
			if rule["type"] == NVBSelectorRule_dict["NVBSelectorRules::TypeID"]:
				return ("Value type", rule["i"].cast("NVBAxisMap::ValueType"))
			if rule["type"] == NVBSelectorRule_dict["NVBSelectorRules::ObjType"]:
				#if rules_type == NVBSelectorRules_dict["NVBSelectorRulesType::Map"]:
					#return ("Map type", rule["i"].cast("NVBAxisMap::MapType"))
				#elif rules_type == NVBSelectorRules_dict["NVBSelectorRulesType::Dataset"]:
					#return ("Dataset type", rule["i"].cast("NVBDataSet::Type"))
				return ("Obj type", rule["i"])
			return ("Unknown", rule["type"])

	def __init__(self, val):
		global NVBSelectorRule_dict
		if not NVBSelectorRule_dict:
			NVBSelectorRule_dict = gdb.types.make_enum_dict(gdb.lookup_type("NVBSelectorRules::NVBSelectorRuleType"))
		self.val = val
		self.container = "QList"
		self.itype = gdb.lookup_type("NVBSelectorRule")

	def children(self):
		try:
			rt = self.val._rules_type
		except Exception:
			rt = 123
		return self._iterator(self.itype, self.val['d'], rt)

	def to_string(self):
		if self.val['d']['end'] == self.val['d']['begin']:
				empty = "empty "
		else:
				empty = ""

		return "%s%s<%s>" % ( empty, self.container, self.itype )

class NVBSelectorDataInstancePrinter(ExpandablePrettyPrinter):
	def children(self):
		return [ \
			("Valid",self.val["valid"]), \
			("Dataset",self.val["dataSet"]), \
			("Selector",self.val["s"]), \
			("Matched axes",self.val["matchedaxes"]), \
			("Other axes",self.val["otheraxes"]), \
			("Matched maps",self.val["matchedmaps"]) \
			]

class NVBSelectorSourceInstancePrinter(ExpandablePrettyPrinter):
	def children(self):
		return [ \
			("Valid",self.val["isValid"]()), \
			("Datasource",self.val["source"]), \
			("Selector",self.val["s"]), \
			("Datainstances",self.val["instances"]), \
			("Indexes",self.val["indexes"]), \
			("Datasets",self.val["sets"]) \
			]

class NVBSelectorFileInstancePrinter(ExpandablePrettyPrinter):
	def children(self):
		return [ \
			("Selector",self.val["s"]), \
			("Sourceinstances",self.val["instances"]), \
			("Indexes",self.val["indexes"]), \
			("Datasources",self.val["sources"]) \
			]

class NVBAssociatedFilesInfoPrinter(ExpandablePrettyPrinter):
	#def __init__(self,val):
		#super(NVBAssociatedFilesInfoPrinter,self).__init__(val)
		#self.strval = "NVBAssociatedFilesInfo"
	
	def children(self):
		return [ \
			("Name",self.val["_name"]), \
			("Generator",self.val["_generator"]), \
			("Filenames",self.val.cast(gdb.lookup_type("QStringList"))) \
			]
		
class NVBFilePrinter(ExpandablePrettyPrinter):
	"""Print NVBFile."""
	def __init__(self, val):
		super(NVBFilePrinter,self).__init__(val)
		self.strval = val["files"]["_name"]
		
	def children(self):
		return [ \
			("Associated files",self.val["files"]), \
			("Comments",self.val["comments"]) \
			]

class NVBDataSourcePrinter(ExpandablePrettyPrinter):
	"""Print NVBDataSource."""
# FIXME preferably show ConstructableDataSource
	def children(self):
		return [ \
			("Comments",self.val["comments"]) \
			]

class NVBDataSetPrinter(ExpandablePrettyPrinter):
	"""Print NVBDataSet."""

	def __init__(self, val):
		super(NVBDataSetPrinter,self).__init__(val)
		self.strval = val["n"]

	def children(self):
		return [ \
			("Type", self.val["t"]), \
			("Units", self.val["dim"]), \
			("Data", self.val["d"]), \
			("Axes", self.val["as"]), \
			("Colors", self.val["clr"]), \
			("Parent", self.val["p"]) \
			]

class NVBAxisPrinter:
	"""Print an NVBAxis."""

	def __init__(self, val):
		self.val = val
		self.nviz = gdb.default_visualizer(val["n"])
	
	#def display_hint(self):
		#return ""
	
	def children(self):
		return [ \
			("Name", self.val["n"]), \
			("Length", self.val["l"]), \
			("Parent", self.val["p"]), \
			("Mappings", self.val["ms"]), \
			("Physmap", self.val["pm"])
			]

	def to_string(self):
		try:
			return "{} x{}".format(self.nviz.to_string(),self.val["l"])
		except Exception:
			return "x{}".format(self.val["l"])

def build_pretty_printer():
	pp = gdb.printing.RegexpCollectionPrettyPrinter("Novembre")

	pp.add_printer('NVBAxisSelector', '^NVBSelectorCase$', NVBAxisSelectorPrinter)
	pp.add_printer('NVBSelectorRules', '^NVBSelectorRules$', NVBSelectorRulesPrinter)
	pp.add_printer('NVBSelectorDataset', '^NVBSelectorDataset$', NVBSelectorDatasetPrinter)
	pp.add_printer('NVBSelectorAxis', '^NVBSelectorAxis$', NVBSelectorAxisPrinter)
	pp.add_printer('NVBSelectorMap', '^NVBSelectorMap$', NVBSelectorMapPrinter)
	pp.add_printer('NVBSelectorRuleList', '^QList<NVBSelectorRule>$', NVBSelectorRuleListPrinter)
	
	pp.add_printer('NVBSelectorDataInstance', '^NVBSelectorDataInstance$', NVBSelectorDataInstancePrinter)
	pp.add_printer('NVBSelectorSourceInstance', '^NVBSelectorSourceInstance$', NVBSelectorSourceInstancePrinter)
	pp.add_printer('NVBSelectorFileInstance', '^NVBSelectorFileInstance$', NVBSelectorFileInstancePrinter)
	
	pp.add_printer('NVBAssociatedFilesInfo', '^NVBAssociatedFilesInfo$', NVBAssociatedFilesInfoPrinter)
	pp.add_printer('NVBFile', '^NVBFile$', NVBFilePrinter)
	pp.add_printer('NVBDataSource', '^NVBDataSource$', NVBDataSourcePrinter)
	pp.add_printer('NVBDataSet', '^NVBDataSet$', NVBDataSetPrinter)
	pp.add_printer('NVBAxis', '^NVBAxis$', NVBAxisPrinter)

	return pp

def deref_printer(val):
	"Look-up and return a pretty-printer that can print val."
	try:
		val_ = val
		type_ = gdb.types.get_basic_type(val_.type)
		if not isinstance(type_, gdb.Type):
			return None
		if type_.code != gdb.TYPE_CODE_PTR:
			return None
		while type_.code == gdb.TYPE_CODE_PTR:
			if val_ == 0:
				return None
			if type_.target().code == gdb.TYPE_CODE_VOID:
				return None
			try:
				val_ = val_.dereference()
			except Exception:
				return None
			type_ = gdb.types.get_basic_type(val_.type)

		return gdb.default_visualizer(val_)
	except Exception:
		gdb.write(traceback.format_exc())
		return None
