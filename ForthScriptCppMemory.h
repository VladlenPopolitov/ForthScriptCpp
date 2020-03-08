#ifndef FORTHSCRIPTCPP_MEMORY_INCLUDED
#define FORTHSCRIPTCPP_MEMORY_INCLUDED


		/****

		Memory Allocation
		-----------------

		By default, ForthScriptCPP's data space is 16K cells.  This may be enough for
		moderate needs, but to process large chunks of data it may be insufficent.  One
		way around this is to define `FORTHSCRIPTCPP_DATASPACE_SIZE` to the size you need,
		but a better solution might be to allocate and free memory as needed.

		****/

/*****
		Maximum chunk size for ALLOCATE and RESIZE.
******/

	const Cell vmMaxSegmentSize = 0x04000000;

		// ALLOCATE ( u -- a-addr ior )
		// @todo reuse of the memory should me proposed and implemented 
		void memAllocate() {
			REQUIRE_DSTACK_DEPTH(1, "ALLOCATE");
			REQUIRE_DSTACK_AVAILABLE(1, "ALLOCATE");
			auto size = SIZE_T(dStack.getTop());
			Cell start = VirtualMemoryFreeSegment;
			dStack.push(0);
			try{
				if (size <= vmMaxSegmentSize){
					VirtualMemoryFreeSegment += size;
					VirtualMemorySegment variables{ start, VirtualMemoryFreeSegment, std::vector<Char>(size) };
					VirtualMemory.push_back(variables);
					dStack.setTop(1,CELL(start));
				}
				else {
					dStack.setTop(1,CELL(0));
					dStack.setTop(errorAllocate);
				}
			}
			catch (...){
				dStack.setTop(1, CELL(0));
				dStack.setTop(errorAllocate);
			}
		}

		// RESIZE ( a-addr1 u -- a-addr2 ior )
		void memResize() {
			REQUIRE_DSTACK_DEPTH(2, "RESIZE");
			Cell size = SIZE_T(dStack.getTop());
			auto addr = dStack.getTop(1);
			// find segment 
			if (size <= vmMaxSegmentSize){
				for (auto i = VirtualMemory.size() - 1; i > vmSegmentDataSpace; --i){
					if (VirtualMemory[i].start == addr){
						auto oldSize = (VirtualMemory[i].end - VirtualMemory[i].start);
						if ( oldSize >= size){
							if (VirtualMemoryFreeSegment == VirtualMemory[i].end){
								VirtualMemoryFreeSegment = VirtualMemory[i].start + size;
							}
							VirtualMemory[i].segment.resize(size);
							VirtualMemory[i].end = VirtualMemory[i].start + size;
							dStack.setTop(0);
							dStack.setTop(1, CELL(addr));
							return;
						}
						else {
							// ( addr size -- )
							// allocate new segment instead of current
							memAllocate();
							if (dStack.getTop(0) == 0){
								// (addr addrnew ior -- )
								drop();
								// (addr addrnew -- )
								dup2();
								// (addr addrnew addr addrnew -- )
								dStack.push(oldSize);
								// (addr addrnew addr addrnew u -- )
								cMove();
								// (addr addrnew -- )
								swap();
								// ( addrnew addr -- )
								memFree();
								// ( addrnew  ior(free) -- )
								dStack.setTop(0);
							}
							else {
								// (addr addrnew ior -- )
								pop();
								// (addr addrnew -- )
								dStack.setTop(errorResize);
								// (addr ior -- )
							}
							return;
						}
					}
				}
			}
			// did not found : ( addr size -- 0 -61 )
			dStack.setTop(static_cast<Cell>(errorResize));
				return;
		}

		// FREE ( a-addr -- ior )
		void memFree() {
			REQUIRE_DSTACK_DEPTH(1, "FREE");
			auto addr = dStack.getTop();
			for (auto i = VirtualMemory.size() - 1; i > vmSegmentDataSpace; --i){
				if (VirtualMemory[i].start == addr){
					if (VirtualMemoryFreeSegment == VirtualMemory[i].end){
						VirtualMemoryFreeSegment = VirtualMemory[i].start;
					}
					if (i < VirtualMemory.size() - 1){
						std::swap(VirtualMemory[i], VirtualMemory[VirtualMemory.size() - 1]);
					}
					VirtualMemory.resize(VirtualMemory.size() - 1);

					dStack.setTop(0);
					return;
				}
			}
			dStack.setTop(errorFree);
		}

#endif
