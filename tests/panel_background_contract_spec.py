#!/usr/bin/env python3
"""Check the authored and generated background contract for every base panel."""
from pathlib import Path
import re
import unittest
import xml.etree.ElementTree as ET

ROOT = Path(__file__).resolve().parents[1]


def layer(root):
    return [e for e in root.iter() if e.get('id') == 'theme_background']


class PanelBackgroundContract(unittest.TestCase):
    def test_every_base_has_a_black_separable_background(self):
        masters = [p for p in (ROOT / 'res').glob('*.svg') if '.' not in p.stem]
        self.assertTrue(masters)
        for path in masters:
            with self.subTest(panel=path.name):
                master = ET.parse(path).getroot()
                backgrounds = layer(master)
                self.assertEqual(len(backgrounds), 1)
                for shape in backgrounds[0].iter():
                    if shape.tag.endswith('g'):
                        continue
                    self.assertEqual(shape.tag.rsplit('}', 1)[-1], 'rect')
                    style = dict(item.split(':', 1) for item in shape.get('style', '').split(';') if ':' in item)
                    self.assertEqual(style.get('fill', shape.get('fill')), '#000000')
                    self.assertEqual(float(style.get('fill-opacity', shape.get('fill-opacity', '1'))), 1)
                panel = ET.parse(path.with_suffix('.panel.svg')).getroot()
                background = ET.parse(path.with_suffix('.background.svg')).getroot()
                self.assertFalse(layer(panel))
                self.assertEqual(len(layer(background)), 1)
                for generated in (panel, background):
                    for attr in ('width', 'height', 'viewBox'):
                        self.assertEqual(generated.get(attr), master.get(attr))
                # The full transform/style ancestor chain must survive extraction.
                def ancestry(root):
                    def walk(node, parents):
                        if node.get('id') == 'theme_background':
                            return parents + [dict(node.attrib)]
                        for child in node:
                            found = walk(child, parents + [dict(node.attrib)])
                            if found is not None:
                                return found
                    return walk(root, [])[1:]  # root ID is generated
                self.assertEqual(ancestry(background), ancestry(master))

    def test_runtime_panels_use_theme_entry_points(self):
        for path in (ROOT / 'src').glob('*.cpp'):
            with self.subTest(source=path.name):
                self.assertIsNone(re.search(r'setPanel\s*\(\s*createPanel\s*\(', path.read_text(encoding='utf-8')))


if __name__ == '__main__':
    unittest.main()
