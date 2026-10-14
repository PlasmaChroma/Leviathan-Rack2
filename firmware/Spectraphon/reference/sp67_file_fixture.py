"""Explicit in-memory file-operation substitutions for original WAV callers.

This is a test fixture, NOT an implementation or emulation of FatFs, storage,
driver errors or physical media. Original caller instructions run to each file
function entry; the fixture supplies bytes/status and resumes at LR. Substituted
function instructions are excluded from the collected instruction trace.
"""
from unicorn import arm_const as arm


class MemoryReadFileFixture:
    READ = 0x0802b264
    SEEK = 0x0802b944
    BOUNDARIES = (READ, SEEK)

    def __init__(self, machine, file_object, data, failures=None):
        self.machine = machine
        self.file_object = file_object
        self.data = bytes(data)
        self.failures = dict(failures or {}) # zero-based I/O operation -> status
        self.position = 0
        self.operations = []
        self.trace = set()
        machine.word(file_object + 12, len(self.data))
        machine.word(file_object + 24, 0)

    def run(self, start, stop, max_calls=128, instruction_limit=10000):
        m = self.machine
        for _ in range(max_calls + 1):
            m.run(start, (*self.BOUNDARIES, stop), instruction_limit)
            self.trace.update(m.last_trace)
            boundary = m.uc.reg_read(arm.UC_ARM_REG_PC)
            if boundary == stop:
                return m.r(0)
            if len(self.operations) >= max_calls:
                raise RuntimeError('File fixture operation limit exceeded')
            if m.r(0) != self.file_object:
                raise ValueError('Unexpected file object at explicit I/O boundary')
            status = self.failures.get(len(self.operations), 0)
            self.service(boundary, status)
            m.word(self.file_object + 12, len(self.data))
            m.word(self.file_object + 24, self.position)
            # Explicit caller-clobbered register values catch accidental reliance
            # on values a C ABI file function is allowed to destroy.
            start = m.r(14) & ~1
            for register in (1, 2, 3):
                m.r(register, 0xa5000000 + register)
            m.r(0, status)

    def service(self, boundary, status):
        m = self.machine
        if boundary == self.SEEK:
            requested = m.r(1)
            # Chosen read-only fixture policy: a seek past EOF lands at EOF.
            if status == 0:
                self.position = min(requested, len(self.data))
            self.operations.append(('seek', requested, self.position, status))
        else:
            destination, requested, count_pointer = m.r(1), m.r(2), m.r(3)
            if requested > 4096:
                raise ValueError('Read exceeds bounded fixture contract')
            chunk = b'' if status else bytes(self.data[self.position:self.position + requested])
            self.operations.append(('read', self.position, requested, len(chunk), status))
            if chunk:
                m.uc.mem_write(destination, chunk)
            m.word(count_pointer, len(chunk))
            self.position += len(chunk)


class MemoryWriteFileFixture(MemoryReadFileFixture):
    """Explicit writable-file substitutions, with zero-filled bounded growth.

    Open creates/truncates one in-memory file. Close snapshots it in files.
    Seek can extend it; truncate cuts at the current position; sync records an
    operation. These are declared test policies, not recovered media behavior.
    short_writes maps zero-based operation indices to successful byte counts.
    """
    OPEN = 0x0802af74
    WRITE = 0x0802b4cc
    CLOSE = 0x0802b840
    TRUNCATE = 0x0802bf88
    SYNC = 0x0802b794
    BOUNDARIES = (*MemoryReadFileFixture.BOUNDARIES, OPEN, WRITE, CLOSE, TRUNCATE, SYNC)

    def __init__(self, machine, file_object, failures=None, short_writes=None):
        super().__init__(machine, file_object, b'', failures)
        self.data = bytearray()
        self.files = {}
        self.filename = None
        self.short_writes = dict(short_writes or {})

    def grow(self, size):
        if not 0 <= size <= 1024*1024:
            raise ValueError('File size exceeds bounded fixture contract')
        if size > len(self.data):
            self.data.extend(bytes(size-len(self.data)))

    def service(self, boundary, status):
        m = self.machine
        if boundary == self.OPEN:
            name = bytes(m.uc.mem_read(m.r(1),64)).split(b'\0',1)[0]
            if len(name) == 64:
                raise ValueError('Unterminated fixture filename')
            self.operations.append(('open',name.decode('ascii'),m.r(2),status))
            if not status:
                self.filename = name.decode('ascii')
                self.data = bytearray();self.position = 0
        elif boundary == self.SEEK:
            requested = m.r(1)
            if not status:
                self.grow(requested);self.position = requested
            self.operations.append(('seek',requested,self.position,status))
        elif boundary == self.WRITE:
            source,requested,count_pointer = m.r(1),m.r(2),m.r(3)
            if requested > 4096:
                raise ValueError('Write exceeds bounded fixture contract')
            count = 0 if status else self.short_writes.get(len(self.operations),requested)
            if not 0 <= count <= requested:
                raise ValueError('Invalid fixture short-write count')
            self.operations.append(('write',self.position,requested,count,status))
            if count:
                self.grow(self.position+count)
                self.data[self.position:self.position+count] = m.uc.mem_read(source,count)
            self.position += count;m.word(count_pointer,count)
        elif boundary == self.TRUNCATE:
            self.operations.append(('truncate',self.position,status))
            if not status:del self.data[self.position:]
        elif boundary == self.SYNC:
            self.operations.append(('sync',self.position,status))
        elif boundary == self.CLOSE:
            self.operations.append(('close',self.position,status))
            if not status:self.files[self.filename] = bytes(self.data)
        else:
            super().service(boundary,status)
