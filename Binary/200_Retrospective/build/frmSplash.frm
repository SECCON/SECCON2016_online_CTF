VERSION 5.00
Begin VB.Form frmSplash 
   BorderStyle     =   3  'ŒÅ’èÀÞ²±Û¸Þ
   ClientHeight    =   4230
   ClientLeft      =   255
   ClientTop       =   1410
   ClientWidth     =   7395
   ClipControls    =   0   'False
   ControlBox      =   0   'False
   Icon            =   "frmSplash.frx":0000
   KeyPreview      =   -1  'True
   LinkTopic       =   "Form2"
   MaxButton       =   0   'False
   MinButton       =   0   'False
   ScaleHeight     =   4230
   ScaleWidth      =   7395
   ShowInTaskbar   =   0   'False
   StartUpPosition =   2  '‰æ–Ê‚Ì’†‰›
   Begin VB.Frame Frame1 
      Height          =   4050
      Left            =   150
      TabIndex        =   0
      Top             =   60
      Width           =   7080
      Begin VB.Image imgLogo 
         Height          =   2385
         Left            =   360
         Picture         =   "frmSplash.frx":000C
         Stretch         =   -1  'True
         Top             =   795
         Width           =   1815
      End
      Begin VB.Label lblCompany 
         Caption         =   "2016 SECCON Incorporated."
         Height          =   255
         Left            =   4560
         TabIndex        =   2
         Top             =   3320
         Width           =   2415
      End
      Begin VB.Label lblVersion 
         Alignment       =   1  '‰E‘µ‚¦
         AutoSize        =   -1  'True
         Caption         =   "Version 1.0"
         BeginProperty Font 
            Name            =   "‚l‚r ‚oƒSƒVƒbƒN"
            Size            =   12
            Charset         =   128
            Weight          =   700
            Underline       =   0   'False
            Italic          =   0   'False
            Strikethrough   =   0   'False
         EndProperty
         Height          =   240
         Left            =   5580
         TabIndex        =   3
         Top             =   2700
         Width           =   1275
      End
      Begin VB.Label lblPlatform 
         Alignment       =   1  '‰E‘µ‚¦
         AutoSize        =   -1  'True
         Caption         =   "FOR WINDOWS"
         BeginProperty Font 
            Name            =   "‚l‚r ‚oƒSƒVƒbƒN"
            Size            =   15.75
            Charset         =   128
            Weight          =   700
            Underline       =   0   'False
            Italic          =   0   'False
            Strikethrough   =   0   'False
         EndProperty
         Height          =   315
         Left            =   4620
         TabIndex        =   4
         Top             =   2340
         Width           =   2235
      End
      Begin VB.Label lblProductName 
         AutoSize        =   -1  'True
         Caption         =   "FLAG CHECKER"
         BeginProperty Font 
            Name            =   "‚l‚r ‚oƒSƒVƒbƒN"
            Size            =   27.75
            Charset         =   128
            Weight          =   700
            Underline       =   0   'False
            Italic          =   0   'False
            Strikethrough   =   0   'False
         EndProperty
         Height          =   555
         Left            =   2520
         TabIndex        =   6
         Top             =   1140
         Width           =   4080
      End
      Begin VB.Label lblLicenseTo 
         Alignment       =   1  '‰E‘µ‚¦
         Caption         =   "Licensed by SECCON"
         Height          =   255
         Left            =   120
         TabIndex        =   1
         Top             =   240
         Width           =   6855
      End
      Begin VB.Label lblCompanyProduct 
         AutoSize        =   -1  'True
         Caption         =   "SECCON SOFTWARE"
         BeginProperty Font 
            Name            =   "‚l‚r ‚oƒSƒVƒbƒN"
            Size            =   18
            Charset         =   128
            Weight          =   700
            Underline       =   0   'False
            Italic          =   0   'False
            Strikethrough   =   0   'False
         EndProperty
         Height          =   360
         Left            =   2355
         TabIndex        =   5
         Top             =   705
         Width           =   3510
      End
   End
End
Attribute VB_Name = "frmSplash"
Attribute VB_GlobalNameSpace = False
Attribute VB_Creatable = False
Attribute VB_PredeclaredId = True
Attribute VB_Exposed = False

Option Explicit

Private Sub Form_KeyPress(KeyAscii As Integer)
    Unload Me
End Sub

Private Sub Frame1_Click()
    Unload Me
    Form1.Show
End Sub

