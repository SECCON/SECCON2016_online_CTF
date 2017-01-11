VERSION 5.00
Begin VB.Form Form1 
   BackColor       =   &H00FFFFC0&
   Caption         =   "Your trial version has expired! --- SECCON FLAG CHECKER."
   ClientHeight    =   4440
   ClientLeft      =   120
   ClientTop       =   450
   ClientWidth     =   7515
   LinkTopic       =   "Form1"
   ScaleHeight     =   4440
   ScaleWidth      =   7515
   StartUpPosition =   3  'Windows ÇÃä˘íËíl
   Begin VB.TextBox Text1 
      Height          =   270
      Left            =   360
      MaxLength       =   28
      TabIndex        =   3
      Top             =   960
      Width           =   6735
   End
   Begin VB.CommandButton Command2 
      BackColor       =   &H008080FF&
      Caption         =   "Exit"
      BeginProperty Font 
         Name            =   "ÇlÇr ÇoÉSÉVÉbÉN"
         Size            =   18
         Charset         =   128
         Weight          =   400
         Underline       =   0   'False
         Italic          =   0   'False
         Strikethrough   =   0   'False
      EndProperty
      Height          =   2535
      Left            =   3960
      Style           =   1  '∏ﬁ◊Ã®Ø∏Ω
      TabIndex        =   2
      Top             =   1320
      Width           =   3375
   End
   Begin VB.CommandButton Command1 
      Appearance      =   0  'Ã◊Øƒ
      BackColor       =   &H00FFC0C0&
      Caption         =   "OK"
      BeginProperty Font 
         Name            =   "ÇlÇr ÇoÉSÉVÉbÉN"
         Size            =   20.25
         Charset         =   128
         Weight          =   400
         Underline       =   0   'False
         Italic          =   0   'False
         Strikethrough   =   0   'False
      EndProperty
      Height          =   2535
      Left            =   120
      MaskColor       =   &H00C0E0FF&
      Style           =   1  '∏ﬁ◊Ã®Ø∏Ω
      TabIndex        =   1
      Top             =   1320
      Width           =   3375
   End
   Begin VB.Label Label2 
      BackColor       =   &H00FFFFFF&
      Caption         =   "15-day trial period has expired!"
      BeginProperty Font 
         Name            =   "ÇlÇr ÇoÉSÉVÉbÉN"
         Size            =   14.25
         Charset         =   128
         Weight          =   400
         Underline       =   0   'False
         Italic          =   0   'False
         Strikethrough   =   0   'False
      EndProperty
      Height          =   375
      Left            =   240
      TabIndex        =   4
      Top             =   3960
      Width           =   7095
   End
   Begin VB.Label Label1 
      Alignment       =   2  'íÜâõëµÇ¶
      BackColor       =   &H0080C0FF&
      BorderStyle     =   1  'é¿ê¸
      Caption         =   "INPUT YOUR LICENSE KEY"
      BeginProperty Font 
         Name            =   "ÇlÇr ÇoÉSÉVÉbÉN"
         Size            =   27.75
         Charset         =   128
         Weight          =   400
         Underline       =   0   'False
         Italic          =   0   'False
         Strikethrough   =   0   'False
      EndProperty
      Height          =   615
      Left            =   120
      TabIndex        =   0
      Top             =   120
      Width           =   7215
   End
End
Attribute VB_Name = "Form1"
Attribute VB_GlobalNameSpace = False
Attribute VB_Creatable = False
Attribute VB_PredeclaredId = True
Attribute VB_Exposed = False
Private Sub Command1_Click()
    If Len(Text1.Text) < 8 Or Left(Text1.Text, 7) <> "SECCON{" Or Right(Text1.Text, 1) <> "}" Then
        GoTo Error
    End If
    
    E = Split(Text1.Text, "_")
    If UBound(E) >= 2 Then
        If Mid(E(0), 8, 1) <> "L" _
            Or Mid(E(0), 9, 1) <> "E" _
            Or Mid(E(0), 11, 1) <> "A" _
            Or Mid(E(0), 10, 1) <> "G" _
            Or Mid(E(0), 13, 1) <> "Y" _
            Or Mid(E(0), 12, 1) <> "C" Then
            GoTo Error
        End If

        J = 0
        For I = 0 To Len(E(1)) - 1
            J = J + (Asc(Mid(E(1), I + 1, 1)) * (4 ^ I))
        Next
        
        If J <> 350 Or Len(E(1)) <> 2 Then
            GoTo Error
        End If
        
        J = J * 256
        
        For I = 0 To Len(E(2)) - 1
            J = J + (Asc(Mid(E(1), I + 1, 1)) * (4 ^ I))
        Next
        
        If J <> 89686 Then
            GoTo Error
        End If
        
        J = 0
        
        For I = 0 To Len(Text1.Text) - 1
            J = J + (Asc(Mid(Text1.Text, I + 1, 1)) * (2 ^ I))
        Next
        
        
    
        If J = &H620F3671 And HexDefaultHash(StrConv(Text1.Text, vbFromUnicode)) = "8B292F1A-9C4631B3-E13CD49C-64EF7454-0352D0C0" Then
            MsgBox "Thank you for your purchase :-) " & vbCrLf & "And, " & Text1.Text & " is Flag.", vbOKOnly, ""
            GoTo Success
        End If

    End If

Error:
    MsgBox "Sorry, This key is not valid.", vbOKOnly, ""
    
Success:
End Sub

Private Sub Command2_Click()
    'Unload Me
    'End
    
    N = HexDefaultHash(StrConv("SECCON{LEGACY_VB_P_CODE}", vbFromUnicode))
End Sub

Private Sub Text1_KeyPress(KeyAscii As Integer)
    If Not (KeyAscii >= 65 And KeyAscii <= 90 Or KeyAscii = 95 Or KeyAscii = 8 Or KeyAscii = 123 Or KeyAscii = 125) Then
        Beep
        KeyAscii = 0
    End If
End Sub
