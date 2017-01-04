
using System;

namespace Net.Sf.Pkcs11
{
	/// <summary>
	/// Description of Slot.
	/// </summary>
	public class Slot
	{
		Module m;
		
		public Module Module {
			get { return m; }
		}
		uint slotId;
		
		public uint SlotId {
			get { return slotId; }
		}
		
		public SlotInfo SlotInfo {
			get{
				return new SlotInfo( m.P11Module.GetSlotInfo(slotId) );
			}
		}
		
		public Token Token{
			get{
				Token localToken = null;

				if (SlotInfo.IsTokenPresent) {
					localToken = new Token(this);
				}

				return localToken;
			}
		}
		
		public Slot(Module m, uint slotId){
			this.m=m;
			this.slotId=slotId;
		}
	}
}
