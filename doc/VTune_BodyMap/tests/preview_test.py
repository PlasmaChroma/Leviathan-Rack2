#!/usr/bin/env python3
"""Optional offline preview test. Requires playwright and a local Chromium."""
from pathlib import Path
import os
import shutil
from playwright.sync_api import sync_playwright
ROOT=Path(__file__).resolve().parents[1]
OUT=ROOT/'build'; OUT.mkdir(exist_ok=True)
with sync_playwright() as p:
    executable = os.environ.get('CHROMIUM_EXECUTABLE') or shutil.which('chromium') or shutil.which('chromium-browser') or shutil.which('google-chrome')
    options = {'headless': True}
    if executable:
        options['executable_path'] = executable
    # Otherwise use the browser installed with `python -m playwright install chromium`.
    browser=p.chromium.launch(**options)
    page=browser.new_page(viewport={'width':1180,'height':1060},device_scale_factor=1)
    errors=[]; page.on('pageerror',lambda err:errors.append(str(err)))
    page.set_content((ROOT/'VTune_BodyMap_Preview.html').read_text(), wait_until='load')
    page.wait_for_function('window.bodyMapReady === true')
    page.wait_for_timeout(650)
    assert '174.00 Hz' in page.locator('#frequencyLabel').inner_text()
    assert page.evaluate('window.bodyMapWeights[3] > .99')
    assert page.evaluate('bodyMapEvaluate(0,0).every(x => x === 0)')
    assert page.evaluate('bodyMapEvaluate(300,0)[3] === 0.5 && bodyMapEvaluate(300,0)[4] === 0.5')
    page.screenshot(path=str(OUT/'preview_174_report.png'),full_page=True)
    page.locator('#mode').select_option('1')
    page.locator('#frequency').fill('528');page.locator('#frequency').dispatch_event('input')
    page.wait_for_timeout(800)
    assert page.evaluate('window.bodyMapWeights[9] > .99')
    page.screenshot(path=str(OUT/'preview_528_symbolic.png'),full_page=True)
    page.locator('#connected').uncheck();page.wait_for_timeout(1100)
    assert page.evaluate('window.bodyMapWeights.every(x => x < .0001)')
    page.locator('#connected').check();page.locator('#mode').select_option('0')
    page.locator('#frequency').fill('45');page.locator('#frequency').dispatch_event('input')
    page.wait_for_timeout(800)
    assert page.evaluate('window.bodyMapWeights[1] > .99')
    page.locator('#light').check();page.wait_for_timeout(300)
    assert page.evaluate("getComputedStyle(document.querySelector('.stage')).backgroundColor === 'rgb(220, 226, 232)'")
    page.screenshot(path=str(OUT/'preview_45_light.png'),full_page=True)
    assert not errors,errors
    browser.close()
print('PASS: local Chromium load, report/symbolic selection, exact band boundary, disconnect fade, light surround, no JavaScript exceptions.')
