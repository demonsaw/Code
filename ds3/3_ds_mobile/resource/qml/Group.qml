
import QtQuick 2.7
import QtQuick.Controls 2.0
import QtGraphicalEffects 1.0
import QtQuick.Layouts 1.0

import "delegate"
import "header"
import "menu"

Page
{
	// Signal
	signal activate();
	onActivate:
	{
		id_view.positionViewAtBeginning();
		id_view.currentIndex = 0;
	}

	// Loader
	Loader
	{
        id: id_loader;
        active: id_swipe_view.currentIndex == page_group; // Close loader(dialog) when user swipes away.
		focus:true;

		// Variables
		property bool valid: item !== null
	}

	// Header
	header: GroupHeader
	{
		id: id_menu_header;
		anchors.left: parent.left;
		anchors.top: parent.top;
		height: header_size;
		width: parent.width;
	}

	Component
	{
		id: id_group_delegate;
		GroupDelegate {}
	}

	// Body
	Rectangle
	{
		anchors.fill: parent;
		color: white;

		// Size
		Text
		{
            id: id_number;
			anchors
			{
				fill: parent;
				topMargin: id_background.height * 0.45;
			}
			color: watermark_color;
			font
			{
				bold: true;
				pixelSize: title_size;
			}
			height: parent.height;
			horizontalAlignment: Text.AlignHCenter;
			opacity: size_opacity;
			text: Number.fromLocaleString(Group.size);
			verticalAlignment: Text.AlignVCenter;
			width: parent.width;
		}

        // Background
        Text
        {
            id: id_background;
            anchors
            {
                verticalCenter: parent.verticalCenter;
                horizontalCenter: id_number.horizontalCenter;
            }
            color: watermark_color;
            font
            {
                bold: true;
                family: "FontAwesome";
                pixelSize: background_size;
            }
            opacity: watermark_opacity;
			rightPadding: font_size / 2;
            text: fa_user;
        }

		// PM Index
		ListView
		{
			id: id_pre_view;

			anchors
			{
				left: parent.left;
				right: parent.right;
				top: parent.top;
			}
			boundsBehavior: Flickable.StopAtBounds;			
			clip: true;
			delegate: Loader
			{
				anchors 
				{
					left: parent.left;
					right: parent.right;
				}

				source: "qrc:/qml/delegate/EnvelopeDelegate.qml";
			}		
			height: (Envelope.size * header_size) + id_separator.height;
			model: envelope_model;
			orientation: Qt.Vertical;
			width: parent.width;
		}

		Rectangle
		{
			id: id_separator;
			anchors
			{				
				left: parent.left;
				right: parent.right;
				top: id_pre_view.bottom;
			}
            color: "#F4F4F4";
			height: (Envelope.size > 0) ? line_size : 0;
			width: parent.width;
			visible: Envelope.size > 0;
		}
		
		// View
		ListView
		{
			id: id_view;

			anchors
			{				
				bottom: parent.bottom;
				bottomMargin: padding_size;
				left: parent.left;
				right: parent.right;
				top: id_separator.bottom;
			}
			boundsBehavior: Flickable.DragOverBounds;
			cacheBuffer: id_root.height;
			clip: true;
			delegate: Loader
			{
				anchors 
				{
					left: parent.left;
					right: parent.right;
				}

				source: "qrc:/qml/delegate/GroupDelegate.qml";
			}
			height: parent.height - id_pre_view.height - id_separator.height;
			maximumFlickVelocity: max_vertical_velocity;
			model: group_model;
			orientation: Qt.Vertical;
			width: parent.width;

			// Header
			GroupListHeader
			{
				id: id_header;
				y: -id_view.contentY - height;
			}
			
			// Footer
			footer: Rectangle
			{
				color: "transparent";
				height: padding_size_2 + 1;
				width: parent.width;
			}

			onDragEnded:
			{
				if (id_header.modified)
					Group.restart();
			}
		}
	}
}
