# Build and test commands for the trading system.
# Run these from this folder, inside WSL (Ubuntu). Example: make cpp-test

CPP_BUILD_DIR := cpp/build
PYTHON_VENV   := python/.venv
GENERATED_DIR := generated
PROTO_FILES   := $(shell find proto -name '*.proto' 2>/dev/null)

.PHONY: cpp-build cpp-test py-install py-test proto clean

# Configure (the first run downloads GoogleTest) and compile the C++ code.
cpp-build:
	cmake -S cpp -B $(CPP_BUILD_DIR) -DCMAKE_BUILD_TYPE=Debug
	cmake --build $(CPP_BUILD_DIR)

# Build, then run every registered C++ test. No tests yet is not a failure.
cpp-test: cpp-build
	ctest --test-dir $(CPP_BUILD_DIR) --output-on-failure --no-tests=ignore

# Create the Python virtual environment and install all dependency groups.
py-install:
	python3 -m venv $(PYTHON_VENV)
	$(PYTHON_VENV)/bin/pip install --upgrade pip
	$(PYTHON_VENV)/bin/pip install -e "python[core,data,research,dev]"

# Run the Python tests. pytest exits with code 5 when it finds no tests,
# which is treated as success until tests exist.
py-test:
	cd python && .venv/bin/pytest; status=$$?; \
	if [ $$status -eq 5 ]; then echo "No Python tests yet."; exit 0; fi; \
	exit $$status

# Generate C++ and Python code from the .proto files in proto/.
proto:
ifeq ($(strip $(PROTO_FILES)),)
	@echo "No .proto files in proto/. Nothing to generate."
else
	mkdir -p $(GENERATED_DIR)/cpp $(GENERATED_DIR)/python
	protoc --proto_path=proto \
		--cpp_out=$(GENERATED_DIR)/cpp \
		--python_out=$(GENERATED_DIR)/python \
		$(PROTO_FILES)
endif

# Delete build output and generated code. Leaves the Python environment alone.
clean:
	rm -rf $(CPP_BUILD_DIR) $(GENERATED_DIR)
