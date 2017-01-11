Attribute VB_Name = "calchash"
 Option Explicit
 
 Private Type FourBytes
     A As Byte
     B As Byte
     C As Byte
     D As Byte
 End Type
 
 Private Type OneLong
     L As Long
 End Type
 
 Function HexDefaultHash(Message() As Byte) As String
 Dim H1 As Long, H2 As Long, H3 As Long, H4 As Long, H5 As Long
 DefaultHash Message, H1, H2, H3, H4, H5
 HexDefaultHash = DecToHex5(H1, H2, H3, H4, H5)
 End Function
 
 
 Sub DefaultHash(Message() As Byte, H1 As Long, H2 As Long, H3 As Long, H4 As Long, H5 As Long)
 Hash Message, &H5A827999, &H6ED9EBA1, &H8F1BBCDC, &HCA62C1D6, H1, H2, H3, H4, H5
 End Sub
 
 Sub Hash(Message() As Byte, ByVal Key1 As Long, ByVal Key2 As Long, ByVal Key3 As Long, ByVal Key4 As Long, H1 As Long, H2 As Long, H3 As Long, H4 As Long, H5 As Long)

 Dim U As Long, P As Long
 Dim FB As FourBytes, OL As OneLong
 Dim I As Integer
 Dim W(80) As Long
 Dim A As Long, B As Long, C As Long, D As Long, E As Long
 Dim T As Long
 
 H1 = &H67452301: H2 = &HEFCDAB89: H3 = &H98BADCFE: H4 = &H10325476: H5 = &HC3D2E1F0
 
 U = UBound(Message) + 1: OL.L = U32ShiftLeft3(U): A = U \ &H20000000: LSet FB = OL 'U32ShiftRight29(U)
 
 ReDim Preserve Message(0 To (U + 8 And -64) + 63)
 Message(U) = 128
 
 U = UBound(Message)
 Message(U - 4) = A
 Message(U - 3) = FB.D
 Message(U - 2) = FB.C
 Message(U - 1) = FB.B
 Message(U) = FB.A
 
 While P < U
     For I = 0 To 15
         FB.D = Message(P)
         FB.C = Message(P + 1)
         FB.B = Message(P + 2)
         FB.A = Message(P + 3)
         LSet OL = FB
         W(I) = OL.L
         P = P + 4
     Next I
 
     For I = 16 To 79
         W(I) = U32RotateLeft1(W(I - 3) Xor W(I - 8) Xor W(I - 14) Xor W(I - 16))
     Next I
 
     A = H1: B = H2: C = H3: D = H4: E = H5
 
     For I = 0 To 19
         T = U32Add(U32Add(U32Add(U32Add(U32RotateLeft5(A), E), W(I)), Key1), ((B And C) Or ((Not B) And D)))
         E = D: D = C: C = U32RotateLeft30(B): B = A: A = T
     Next I
     For I = 20 To 39
         T = U32Add(U32Add(U32Add(U32Add(U32RotateLeft5(A), E), W(I)), Key2), (B Xor C Xor D))
         E = D: D = C: C = U32RotateLeft30(B): B = A: A = T
     Next I
     For I = 40 To 59
         T = U32Add(U32Add(U32Add(U32Add(U32RotateLeft5(A), E), W(I)), Key3), ((B And C) Or (B And D) Or (C And D)))
         E = D: D = C: C = U32RotateLeft30(B): B = A: A = T
     Next I
     For I = 60 To 79
         T = U32Add(U32Add(U32Add(U32Add(U32RotateLeft5(A), E), W(I)), Key4), (B Xor C Xor D))
         E = D: D = C: C = U32RotateLeft30(B): B = A: A = T
     Next I
     
     H1 = U32Add(H1, A): H2 = U32Add(H2, B): H3 = U32Add(H3, C): H4 = U32Add(H4, D): H5 = U32Add(H5, E)
 Wend
 End Sub
 
 Function U32Add(ByVal A As Long, ByVal B As Long) As Long
 If (A Xor B) < 0 Then
     U32Add = A + B
 Else
     U32Add = (A Xor &H80000000) + B Xor &H80000000
 End If
 End Function
 
 Function U32ShiftLeft3(ByVal A As Long) As Long
 U32ShiftLeft3 = (A And &HFFFFFFF) * 8
 If A And &H10000000 Then U32ShiftLeft3 = U32ShiftLeft3 Or &H80000000
 End Function
 
 Function U32ShiftRight29(ByVal A As Long) As Long
 U32ShiftRight29 = (A And &HE0000000) \ &H20000000 And 7
 End Function
 
 Function U32RotateLeft1(ByVal A As Long) As Long
 U32RotateLeft1 = (A And &H3FFFFFFF) * 2
 If A And &H40000000 Then U32RotateLeft1 = U32RotateLeft1 Or &H80000000
 If A And &H80000000 Then U32RotateLeft1 = U32RotateLeft1 Or 1
 End Function
 
 Function U32RotateLeft5(ByVal A As Long) As Long
 U32RotateLeft5 = (A And &H3FFFFFF) * 32 Or (A And &HF8000000) \ &H8000000 And 31
 If A And &H4000000 Then U32RotateLeft5 = U32RotateLeft5 Or &H80000000
 End Function
 
 Function U32RotateLeft30(ByVal A As Long) As Long
 U32RotateLeft30 = (A And 1) * &H40000000 Or (A And &HFFFC) \ 4 And &H3FFFFFFF
 If A And 2 Then U32RotateLeft30 = U32RotateLeft30 Or &H80000000
 End Function
 
 Function DecToHex5(ByVal H1 As Long, ByVal H2 As Long, ByVal H3 As Long, ByVal H4 As Long, ByVal H5 As Long) As String
 Dim H As String, L As Long
 DecToHex5 = "00000000-00000000-00000000-00000000-00000000"
 H = Hex(H1): L = Len(H): Mid(DecToHex5, 9 - L, L) = H
 H = Hex(H2): L = Len(H): Mid(DecToHex5, 18 - L, L) = H
 H = Hex(H3): L = Len(H): Mid(DecToHex5, 27 - L, L) = H
 H = Hex(H4): L = Len(H): Mid(DecToHex5, 36 - L, L) = H
 H = Hex(H5): L = Len(H): Mid(DecToHex5, 45 - L, L) = H
 End Function

