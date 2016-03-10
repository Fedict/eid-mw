using System;
using System.Runtime.InteropServices;

namespace Net.Sf.Pkcs11.Wrapper
{
	public class AttributeUtil
	{
		public static CK_ATTRIBUTE CreateClassAttribute(){
			return createAttribute((uint)CKA.CLASS,new byte[0]);
			
		}
		
		public static CK_ATTRIBUTE CreateClassAttribute(CKO objectClass){

			return createAttribute((uint)CKA.CLASS,BitConverter.GetBytes((uint)objectClass));
		}
		
		public static CK_ATTRIBUTE createAttribute(uint type, byte[]val ){
			
			CK_ATTRIBUTE attr= new CK_ATTRIBUTE();
			attr.type=type;
			if(val!=null && val.Length>0){
				attr.ulValueLen=(uint)val.Length;
				attr.pValue=Marshal.AllocHGlobal(val.Length);
				Marshal.Copy(val,0,attr.pValue,val.Length);
			}else{
				attr.ulValueLen=(uint)val.Length;
				attr.pValue=IntPtr.Zero;
				
			}
			return attr;
		}
		
		public static CK_ATTRIBUTE createAttribute(uint type, int size ){
			return createAttribute(type,new byte[size]);
		}
		
		public static CK_ATTRIBUTE createAttribute(CKA type, int size ){
			return createAttribute((uint)type,new byte[size]);
		}
		
	}
}
