//
// The MIT License(MIT)
//
// Copyright(c) 2014 Demonsaw LLC
//
// Permission is hereby granted, free of charge, to any person obtaining a copy
// of this software and associated documentation files (the "Software"), to deal
// in the Software without restriction, including without limitation the rights
// to use, copy, modify, merge, publish, distribute, sublicense, and/or sell
// copies of the Software, and to permit persons to whom the Software is
// furnished to do so, subject to the following conditions:
// 
// The above copyright notice and this permission notice shall be included in all
// copies or substantial portions of the Software.
// 
// THE SOFTWARE IS PROVIDED "AS IS", WITHOUT WARRANTY OF ANY KIND, EXPRESS OR
// IMPLIED, INCLUDING BUT NOT LIMITED TO THE WARRANTIES OF MERCHANTABILITY,
// FITNESS FOR A PARTICULAR PURPOSE AND NONINFRINGEMENT. IN NO EVENT SHALL THE
// AUTHORS OR COPYRIGHT HOLDERS BE LIABLE FOR ANY CLAIM, DAMAGES OR OTHER
// LIABILITY, WHETHER IN AN ACTION OF CONTRACT, TORT OR OTHERWISE, ARISING FROM,
// OUT OF OR IN CONNECTION WITH THE SOFTWARE OR THE USE OR OTHER DEALINGS IN THE
// SOFTWARE.

namespace DemonSaw.Component
{
	using DemonSaw.Collection;
	using DemonSaw.Entity;
	using DemonSaw.Json;
	using System.Diagnostics;

	public sealed class ChunkComponent : EntityComponent
	{
		// Variables
		private Deque<JsonChunk> _chunks;
		private Deque<JsonTransfer> _data;

		// Properties
		public int Chunks { get; private set; }
		public int Remaining { get; private set; }
		public int Size { get; private set; }
		public int Max { get; private set; }

		// Properties - Helper
		public bool Finished { get { return Remaining == 0; } }

		// Constructors
		public ChunkComponent(long total, int size, int max)
		{
			Chunks = (int) (total / size);
			if ((total % size) > 0)
				Chunks++;
			
			Max = max;
			Remaining = Chunks;
			_chunks = new Deque<JsonChunk>(Chunks);
			_data = new Deque<JsonTransfer>(max);

			lock (_chunks)
			{
				for (int i = 0; i < Chunks; ++i)
				{
					long offset = (long) i * (long) size;
					int bytes = ((offset + size) > total) ? (int) (total - offset) : size;
					Debug.Assert(bytes > 0);

					JsonChunk jsonChunk = new JsonChunk() { Offset = offset, Size = bytes };
					PushChunk(jsonChunk);
				}
			}
		}

		// Index
		public void PushChunk(JsonChunk chunk)
		{
			lock (_chunks)
				_chunks.Push(chunk); 
		}

		public JsonChunk PopChunk() 
		{
			lock (_chunks)
				return HasChunk() ? _chunks.Pop() : null;
		}

		public bool HasChunk()
		{
			lock (_chunks)
				return !_chunks.Empty;
		}

		// Data
		public void PushData(JsonTransfer transfer)
		{
			lock (_data)
				_data.Push(transfer);
		}

		public JsonTransfer PopData()
		{
			lock (_data)
			{
				if (HasData())
				{
					--Remaining;
					return _data.Pop();
				}

				return null;
			}				
		}

		public bool HasData()
		{
			lock (_data)
				return !_data.Empty;
		}

		public bool HasDataCapacity()
		{
			lock (_data)
				return _data.Count < Max;
		}
	}
}
