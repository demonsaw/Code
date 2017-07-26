import QtQuick 2.7
import QtGraphicalEffects 1.0

Item {
	id: slider

	height: background.height
	property real value: 0
	property real maximum: 1
	property real minimum: 0
	property int xMax: width - handle.width
	onXMaxChanged: updatePos()
	onMinimumChanged: updatePos()
	onValueChanged: if (!pressed) updatePos()
	property bool mutable: true
	property alias pressed : backgroundMouse.pressed
	property alias hit: hit

	signal valueChangedByHandle(int newValue)

	Rectangle
	{
		id: background
		color: light_grey
		width: parent.availableWidth;
        height: parent.height / 2;
        implicitHeight: parent.height / 2;
		implicitWidth: slider.width
		LinearGradient
		{
			anchors.fill: parent
			start: Qt.point(background.width, background.y)
			end: Qt.point(background.x, background.y)
			gradient: Gradient {
				GradientStop { position: 0.0; color:  Qt.rgba(colorRGBA_bright.x, colorRGBA_bright.y, colorRGBA_bright.z, colorRGBA_bright.w) }
				GradientStop { position: 1.0; color: black}
			}
		}

		// Drop Shadow
		layer.enabled: true;
		layer.effect: DropShadow
		{
			transparentBorder: true;
            color: softest_shadow;
			horizontalOffset: 0;
		}

		Rectangle
		{
			id: hit
			anchors.verticalCenter: parent.verticalCenter;
			width: parent.width;
			height: parent.height * 6;
            x: 0;
            y: 0;
			color: "transparent"

			MouseArea
			{
				id: backgroundMouse
				anchors.fill: parent
				enabled: slider.mutable
				drag.target: handle
				drag.axis: Drag.XAxis
				drag.minimumX: 0
				drag.maximumX: slider.xMax
				onReleased:
				{
					value = Math.max(minimum, Math.min(maximum, (maximum - minimum) * (mouseX - handle.width/2) / slider.xMax + minimum));
					valueChangedByHandle(value);
					updatePos();
				}
				onPositionChanged:
				{
					value = Math.max(minimum, Math.min(maximum, (maximum - minimum) * (mouseX - handle.width/2) / slider.xMax + minimum));
					valueChangedByHandle(value);
					updatePos();
				}
				/*
				onWheel: {
					value = Math.max(minimum, Math.min(maximum, value + (wheel.angleDelta.y > 0 ? 1 : -1) * (10 / slider.xMax) * (slider.maximum - slider.minimum)));
					valueChangedByHandle(value);
					updatePos();
				}
				*/
			}
		}
	}

	Rectangle
	{
		id: handle
		color: Qt.rgba(colorRGBA.x, colorRGBA.y, colorRGBA.z, colorRGBA.w);
        implicitWidth: icon_size;
        implicitHeight: icon_size;
		radius: width / 2;
		anchors.verticalCenter: background.verticalCenter
		visible: slider.enabled

		// Drop Shadow
		layer.enabled: true;
		layer.effect: DropShadow
		{
			transparentBorder: true;
			color: soft_shadow;
			horizontalOffset: 0;
		}
	}

	function updatePos()
	{
		if (maximum > minimum)
		{
			var pos = 0 + (value - minimum) * slider.xMax / (maximum - minimum);
			pos = Math.min(pos, width - handle.width - 0);
			pos = Math.max(pos, 0);
			handle.x = pos;
		} else
		{
			handle.x = 0;
		}
	}

}
