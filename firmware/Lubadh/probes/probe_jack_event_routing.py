#!/usr/bin/env python3
"""Original complete jack producer with explicit inert event-recording boundary."""
import itertools
import json
from arm_byte_probe import ARMBytes, ROOT
from unicorn.arm_const import UC_ARM_REG_PC, UC_ARM_REG_LR

APP = 0x100000
DECKS = (APP + 72, APP + 173440)
LINKS = (0x170000, 0x171000)
PRESETS = (0x180000, 0x181000)
EDGE_FIELDS = (0x19D, 0x19E, 0x1AD, 0x1BD)


class RoutingBytes(ARMBytes):
    def __init__(self):
        super().__init__()
        self.events = []
        self.pin_levels = {}
        self.pin_reads = []

    def _code(self, cpu, address, size, data):
        if address == 0x6C0C4:
            pin = self.reg(1)
            self.pin_reads.append(pin)
            self.reg(0, self.pin_levels[pin])
            cpu.reg_write(UC_ARM_REG_PC, cpu.reg_read(UC_ARM_REG_LR))
            return
        if address == 0x3F1C4:
            self.events.append((DECKS.index(self.reg(0)), self.reg(1)))
            cpu.reg_write(UC_ARM_REG_PC, cpu.reg_read(UC_ARM_REG_LR))
            return
        super()._code(cpu, address, size, data)


def reference(edges, roles, linked, oneshot_modes, oneshot, stop):
    events, shot = [], list(oneshot)
    def send(deck, event):
        events.append((deck, event))
    def both(deck, event):
        send(deck, event)
        if linked:
            send(deck ^ 1, event)
    for deck in range(2):
        rise, fall, erase, retrigger = edges[deck]
        if rise:
            if roles[deck] in (0, 1):
                both(deck, 0 if roles[deck] == 0 else 4)
            elif roles[deck] == 2:
                if not linked:
                    send(deck, 11)
                    if shot[deck]:
                        shot[deck] = 0
                        send(deck, 0)
                elif deck == 0:
                    send(1, 11)
                    send(0, 11)
                    if shot[0]:
                        shot[:] = [0, 0]
                        both(0, 0)
                else:
                    both(1, 0)
        if fall:
            if not linked:
                send(deck, 12)
            elif deck == 0:
                send(1, 12)
                send(0, 12)
            if stop[deck]:
                both(deck, 5)
        if erase:
            both(deck, 23 if oneshot_modes[deck] == 1 else 1)
        if retrigger:
            both(deck, 2)
    return events, shot


def main():
    c = RoutingBytes()
    cases = calls = 0
    traces = []
    def run(mask, roles, linked, oneshot_modes, shot, stop, gpio=False):
        nonlocal cases, calls
        edges = [[(mask >> (deck * 4 + bit)) & 1 for bit in range(4)] for deck in range(2)]
        c.events = []
        c.write(APP + 344064 + 0xABC, bytes([linked]))
        for deck, base in enumerate(DECKS):
            c.putu(base, deck)
            c.putu(base + 232, LINKS[deck])
            c.putu(LINKS[deck] + 176, PRESETS[deck])
            c.putu(PRESETS[deck] + 24, roles[deck])
            c.putu(PRESETS[deck] + 8, oneshot_modes[deck])
            c.write(base + 172, bytes([shot[deck]]))
            c.write(base + 651, bytes([stop[deck]]))
            for field, value in zip(EDGE_FIELDS, edges[deck]):
                if gpio:
                    assert c.read(base + field, 1)[0] == value
                else:
                    c.write(base + field, bytes([value]))
        c.reg(0, APP)
        c.call(0x284B0)
        expected, final_shot = reference(edges, roles, linked, oneshot_modes, shot, stop)
        assert c.events == expected, (mask, roles, linked, oneshot_modes, shot, stop, c.events, expected)
        assert [c.read(base + 172, 1)[0] for base in DECKS] == final_shot
        assert [[c.read(base + field, 1)[0] for field in EDGE_FIELDS] for base in DECKS] == edges
        cases += 1
        calls += len(c.events)
        if mask == 255 and roles in ((2, 2), (0, 1)) and oneshot_modes == (0, 1) and shot == (1, 1) and stop == (1, 1):
            traces.append(dict(roles=roles, linked=linked, events=c.events, final_oneshot=final_shot))
    # Every combination of both decks' four edge flags, every valid role pair,
    # both link states; all other fixtures deliberately asymmetric.
    for mask, roles, linked in itertools.product(range(256), itertools.product(range(3), repeat=2), range(2)):
        run(mask, roles, linked, (0, 1), (1, 1), (1, 1))
    # Full secondary flag matrix at representative individual/simultaneous edges.
    pairs = list(itertools.product(range(2), repeat=2))
    for mask, roles, linked, record, shot, stop in itertools.product(
            (0, 1, 2, 3, 15, 16, 32, 48, 240, 255),
            itertools.product(range(3), repeat=2), range(2), pairs, pairs, pairs):
        run(mask, roles, linked, record, shot, stop)
    # Join original six-read GPIO callback slice and complete Button recurrence
    # to the dispatcher. Pins are explicit synthetic fixture identifiers.
    gpio_cases = 0
    pin_fields = (0x485, 0x489, 0x487)
    button_fields = (0x1B0, 0x1A0, 0x190)
    for old, new in itertools.product(range(64), repeat=2):
        c.pin_levels = {pin: (new >> pin) & 1 for pin in range(6)}
        c.pin_reads = []
        mask = 0
        for deck, base in enumerate(DECKS):
            for slot, (pin_field, button_field) in enumerate(zip(pin_fields, button_fields)):
                pin = deck * 3 + slot
                previous, current = (old >> pin) & 1, (new >> pin) & 1
                c.write(base + 172032 + pin_field, bytes([pin]))
                c.putu(base + button_field + 8, previous)
                c.putu(base + button_field + 4, previous)
                rise, fall = int(current == 1 and previous == 0), int(current == 0 and previous == 1)
                if slot == 2:
                    mask |= rise << (deck * 4)
                    mask |= fall << (deck * 4 + 1)
                else:
                    mask |= rise << (deck * 4 + (3 if slot == 0 else 2))
        c.reg(4, APP)
        c.reg(5, DECKS[1])
        c.reg(8, APP + 520176)
        c.call(0x295CC, stop_before=0x29628)
        assert c.pin_reads == list(range(6))
        for deck, base in enumerate(DECKS):
            for slot, button_field in enumerate(button_fields):
                current = (new >> (deck * 3 + slot)) & 1
                previous = (old >> (deck * 3 + slot)) & 1
                assert c.getu(base + button_field + 4) == current
                assert c.getu(base + button_field + 8) == current
                assert c.read(base + button_field + 13, 2) == bytes([int(current and not previous), int(previous and not current)])
        run(mask, (2, 0), old & 1, (1, 0), (1, 1), (1, 1), gpio=True)
        gpio_cases += 1
    result = dict(status='PASS', routing_cases=cases, recorded_event_calls=calls,
                  joined_gpio_transition_cases=gpio_cases,
                  distinct_producer_instruction_addresses=len(c.coverage), traces=traces,
                  limitations=['Complete interpretJacks; interpretButtonPress replaced by inert event recorder.',
                               'Routing matrix supplies edge flags; joined GPIO matrix uses six synthetic readPin levels and initialized previous levels.',
                               'Preset pointers, link state, one-shot and stop flags supplied.',
                               'Consumer mutations, analog voltage thresholds and audible transport not executed.'])
    (ROOT / 'probes/jack_event_routing_probe_results.json').write_text(json.dumps(result, indent=2) + '\n')
    print(json.dumps({key: value for key, value in result.items() if key != 'traces'}, indent=2))


if __name__ == '__main__':
    main()
