FasdUAS 1.101.10   ��   ��    k             l     ����  O       	  k     
 
     I   	������
�� .miscactvnull��� ��� obj ��  ��     ��  I  
 �� ��
�� .aevtodocnull  �    alis  n   
     4    �� 
�� 
appf  m       �    B E I D T o k e n . a p p  n   
     4    �� 
�� 
cfol  m       �    A p p l i c a t i o n s  1   
 ��
�� 
sdsk��  ��   	 m       �                                                                                  MACS  alis    ~  macOS 10.13 dev            �U��H+   =�
Finder.app                                                      w&�b�>        ����  	                CoreServices    �U��      �b�     =� n� n�  9macOS 10.13 dev:System: Library: CoreServices: Finder.app    
 F i n d e r . a p p     m a c O S   1 0 . 1 3   d e v  &System/Library/CoreServices/Finder.app  / ��  ��  ��     ��  l      ��  ��    � �
set BEIDToken_running to false

repeat 4 times
	if application "BEIDToken" is running then
		tell application "BEIDToken"
			quit
		end tell
		set BEIDToken_running to true
	else
		delay 1
	end if
end repeat

do shell script "Killall BEIDToken"     �  � 
 s e t   B E I D T o k e n _ r u n n i n g   t o   f a l s e 
 
 r e p e a t   4   t i m e s 
 	 i f   a p p l i c a t i o n   " B E I D T o k e n "   i s   r u n n i n g   t h e n 
 	 	 t e l l   a p p l i c a t i o n   " B E I D T o k e n " 
 	 	 	 q u i t 
 	 	 e n d   t e l l 
 	 	 s e t   B E I D T o k e n _ r u n n i n g   t o   t r u e 
 	 e l s e 
 	 	 d e l a y   1 
 	 e n d   i f 
 e n d   r e p e a t 
 
 d o   s h e l l   s c r i p t   " K i l l a l l   B E I D T o k e n "��       ��  ��    ��
�� .aevtoappnull  �   � ****  ��  ���� ! "��
�� .aevtoappnull  �   � ****   k      # #  ����  ��  ��   !   "  ������ �� ��
�� .miscactvnull��� ��� obj 
�� 
sdsk
�� 
cfol
�� 
appf
�� .aevtodocnull  �    alis�� � *j O*�,��/��/j U ascr  ��ޭ