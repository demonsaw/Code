
import QtQuick 2.7
import QtQuick.Controls 2.0
import QtQuick.Layouts 1.0

import "header"

Page
{
    // Variables (easter egg)
	property bool enigma: false
	property bool eijah: false
	property bool cameron: false
	property bool easter: enigma && eijah && cameron

    // Quotes
    property int index: -1;
    property var quotes: ["It's in that place where I put that thing that time.",
		"I hack, therefore I am.",
		"Freedom is not having to ask permission to live our lives.",
		"Friends don't let friends DMCA.",
		"If privacy is outlawed, only outlaws will have privacy.",	
		"Believe in the Right to Share.",		
		"Demonsaw. It's like waking up from a long Napster.",
		"Ratios are DRM for torrents",
		"Every man takes the limits of his own field of vision\n for the limits of the world.", 
		"My shit, my way, fuck off NSA!"];
    property int n: 10;

	// Header
	header: BackHeader
	{
		anchors.left: parent.left;
		anchors.top: parent.top;
		height: header_size;
		width: parent.width;
	}

	Rectangle 
	{
		anchors
		{
			fill: parent;
		}
		color: white;

		Image
		{
			id: id_enigma;
			anchors
			{
				left:parent.left;
				right:parent.right;
				top: parent.top;
				topMargin: header_size;
				bottomMargin: header_size;
			}
			fillMode: Image.Pad;
			source: "qrc:/logo_title.svg";
			sourceSize.width: parent.width;
			sourceSize.height: parent.height * 0.70;

			RotationAnimation on rotation
			{
					from: 0
					to: 720
					running: animate_logo.pressed
					duration: 300
			}

			MouseArea
			{
				id: animate_logo
				anchors.fill:parent
				onClicked: enigma = !enigma;
			}
		}

		// Size
		Text
		{
			anchors
			{
				left: parent.left;
				right: parent.right;
				top: parent.top;				
			}
			color: watermark_color;
			font
			{
				bold: true;
				pixelSize: title_size;
			}
			height: header_size;
			horizontalAlignment: Text.AlignHCenter;
			opacity: size_opacity;
			text: "2 programmers";			
			verticalAlignment: Text.AlignVCenter;
			width: parent.width;
		}

		Item
		{
			id: credit_text
			anchors
			{
				left: parent.left;
				right: parent.right;
				top: id_enigma.bottom;
			}

			height: title_size * 2;
			width: parent.width;

			Text
			{
				anchors.left: parent.left;
				color: black;
				font.bold: true;
				font.pixelSize: about_text_size;
				horizontalAlignment: Text.AlignHCenter;
				text: "eijah";
				width: parent.width / 2;
				verticalAlignment: Text.AlignVCenter;

				RotationAnimation on rotation
				{
						from: 1080
						to: 0
						running: animate_eijah.pressed
						duration: 300
				 }
				SequentialAnimation on color
				{
					ColorAnimation { from: black; to: demonsaw; duration: 500 }
					running: animate_eijah.pressed
				}

				MouseArea
				{
					id: animate_eijah
					anchors.fill:parent
					onClicked: eijah = !eijah
				}
			}

			Text
			{
				anchors.right: parent.right;
				color: blue;
				font.bold: true;
				font.pixelSize: about_text_size;
				horizontalAlignment: Text.AlignHCenter;
				text: "cameron";
				width: parent.width / 2;
				verticalAlignment: Text.AlignVCenter;

				RotationAnimation on rotation
				{
						from: 0
						to: 1080
						running: animate_cameron.pressed
						duration: 300
				}

				SequentialAnimation on color
				{
					ColorAnimation { from: blue; to: demonsaw; duration: 500 }
					running: animate_cameron.pressed
				}

				MouseArea
				{
					id: animate_cameron
					anchors.fill:parent
					onClicked: cameron = !cameron
				}
			}
		}
		Item
		{
            id: id_quote_container;
			anchors
			{
				horizontalCenter: parent.horizontalCenter;
                bottom: id_version.top;
                bottomMargin: margin_height * 2;
			}
			height: title_size
			width: parent.width
			visible: easter

            onVisibleChanged:
            {
                if(visible)
                {
                    index++;
                    id_quote.text = quotes[index % n];
                    if(index == 0)
                        Toast.show("Achievement unlocked");
                }
            }

			Text
			{
                id: id_quote;
                anchors.horizontalCenter: parent.horizontalCenter
                horizontalAlignment: Text.AlignHCenter;
                text: "";
				color: demonsaw;
				font.pixelSize: font_size * 0.75;
                topPadding: margin_height;
				wrapMode: Text.WrapAnywhere
				visible: easter
			}
		}
        // Version
        Text
        {
            id: id_version;
            anchors
            {
                topMargin: margin_height;
                bottom: id_copyright.top;
                bottomMargin: margin_height;
                horizontalCenter: parent.horizontalCenter;
            }
            color: grey;
            font.pixelSize: font_size * 0.75;
            horizontalAlignment: Text.AlignHCenter;
            text: version;
            verticalAlignment: Text.AlignVCenter;
            width: parent.width;
        }

        // Copyright
        Text
        {
            id: id_copyright;
            anchors
            {
                bottom: parent.bottom;
                bottomMargin: margin_height;
                horizontalCenter: parent.horizontalCenter;
            }
            color: grey;
            font.bold: true;
            font.pixelSize: font_size * 0.75;
            horizontalAlignment: Text.AlignHCenter;
            text: copyright;
            verticalAlignment: Text.AlignVCenter;
            width: parent.width;
        }
	}
}
