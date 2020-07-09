import sys
import vdwo
import openbabel as ob

obconv = ob.OBConversion()
obconv.SetInFormat('mol')
mol = ob.OBMol()
obconv.ReadFile(mol, 'C6H6.mol')
print(vdwo.vdwo(mol, mol, float(sys.argv[1])))
