from os import environ, cpu_count
environ['OMP_NUM_THREADS'] = str(cpu_count())
from pathlib import Path
import vdwo
import openbabel as ob

obconv = ob.OBConversion()
obconv.SetInFormat('mol')
root = Path(__file__).resolve().parent
for path in (root / 'mol-files').glob('*.mol'):
    name = path.stem
    mol = ob.OBMol()
    obconv.ReadFile(mol, str(path))
    volume = vdwo.vdwo(mol, mol, 0.01)
    print(f'VdW Volume of {name} is {volume:.1f} \u212b\u00b3')

