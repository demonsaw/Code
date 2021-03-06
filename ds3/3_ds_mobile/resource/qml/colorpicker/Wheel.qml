import QtQuick 2.7
Item {
	id: wheel_root

	property real hue : 1
	property real saturation : 1

	// Signals
	signal accepted()
	signal modified()
	signal updateHS(var hueSignal, var saturationSignal)
	signal initColor()

	// States
	states : [
		// When user is moving the slider
		State {
			name: "editing"
			PropertyChanges {
				target: wheel_root
				// Update
				hue: hue
				saturation: saturation
			}
		}
	]
	// Circular Wheel
	Rectangle {
		id: wheel
		width: parent.width < parent.height ? parent.width : parent.height;
		height: width;
		anchors.horizontalCenter: parent.horizontalCenter
		anchors.verticalCenter: parent.verticalCenter
		color: "transparent"

		// Shaders
		ShaderEffect
		{
			id: shader
			anchors.fill: parent
			vertexShader: "
				uniform highp mat4 qt_Matrix;
				attribute highp vec4 qt_Vertex;
				attribute highp vec2 qt_MultiTexCoord0;
				varying highp vec2 coord;
				void main()
				{
					coord = qt_MultiTexCoord0 - vec2(0.5, 0.5);
					gl_Position = qt_Matrix * qt_Vertex;
				}"
			fragmentShader: "
				varying highp vec2 coord;
				uniform lowp float qt_Opacity; 
				precision highp float; 
				vec3 hsv2rgb(in vec3 c)
				{
					vec4 k = vec4(1.0, 2.0 / 3.0, 1.0 / 3.0, 3.0);
					vec3 p = abs(fract(c.xxx + k.xyz) * 6.0 - k.www);
					return c.z * mix(k.xxx, clamp(p - k.xxx, 0.0, 1.0), c.y);
				}
				void main() {
					const float PI = 3.14159265358979323846264;
					float s = sqrt(coord.x * coord.x + coord.y * coord.y);
					if( s > 0.5 )
					{
						gl_FragColor = vec4(0, 0, 0, 0);
						return;
					}
					float h = - atan( coord.y / coord.x );
					s *= 2.0;
					if( coord.x >= 0.0 )
					{
						h += PI;
					}
					h = h / (2.0 * PI);
					vec3 hsl = vec3(h, s, 1.0);
					vec3 rgb = hsv2rgb(hsl);
					gl_FragColor.rgb = rgb;
					gl_FragColor.a = 1.0;
				}"
		}

		// Inner-circle (color picker)
		Item {
			id: pickerCursor
			x: parent.width / 2 * (1 + wheel_root.saturation * Math.cos(2 * Math.PI * wheel_root.hue - Math.PI)) - r
			y: parent.width / 2 * (1 + wheel_root.saturation * Math.sin(-2 * Math.PI * wheel_root.hue - Math.PI)) - r
			property int r : margin_width * 0.9

			Rectangle
			{
				width: parent.r*2;
				height: parent.r*2;
				radius: parent.r;
				border.color: black;
				border.width: margin_width / 4;
				color: "transparent"

				Rectangle
				{
					anchors.fill: parent;
					anchors.margins: margin_width / 4;
					border.color: white;
					border.width: margin_width / 4;
					radius: width/2
					color: "transparent"
				}
			}
		}

		// Wheel MouseArea
		MouseArea
		{
			id : wheelArea
			preventStealing: true;
			// Bound pickerCursor and update colors
			function keepCursorInWheel(mouse, wheelArea, wheel)
			{
				wheel_root.state = 'editing'
				forceActiveFocus();
				if (mouse.buttons & Qt.LeftButton)
				{
					// Cartesian to polar coords
					var ro = Math.sqrt(Math.pow(mouse.x-wheel.width/2,2)+Math.pow(mouse.y-wheel.height/2,2));
					var theta = Math.atan2(((mouse.y-wheel.height/2)*(-1)),((mouse.x-wheel.width/2)));

					// Wheel limit
					if(ro > wheel.width/2)
						ro = wheel.width/2;

					// Polar to cartesian coords
					var cursor = Qt.vector2d(0, 0);
					cursor.x = Math.max(-pickerCursor.r, Math.min(wheelArea.width, ro*Math.cos(theta)+wheel.width/2)-pickerCursor.r);
					cursor.y = Math.max(-pickerCursor.r, Math.min(wheelArea.height, wheel.height/2-ro*Math.sin(theta)-pickerCursor.r));

					// Update
					hue = Math.ceil((Math.atan2(((cursor.y+pickerCursor.r-wheel.height/2)*(-1)),((cursor.x+pickerCursor.r-wheel.width/2)))/(Math.PI*2)+0.5)*100)/100
					saturation = Math.ceil(Math.sqrt(Math.pow(cursor.x+pickerCursor.r-width/2,2)+Math.pow(cursor.y+pickerCursor.r-height/2,2))/wheel.height*2*100)/100;
					wheel_root.updateHS(hue, saturation);
					var color = Qt.rgba(colorRGBA.x, colorRGBA.y, colorRGBA.z, colorRGBA.w)
					name_text.background.color = color;                    
					selected_color = color;
					if(wizard_active)
					{
						start_button_background.color = color;
						id_wizard_statusbar.color = color;
					}
					// Update font color
					updateTextColor();
				}
			}

			anchors.fill: parent
			onPositionChanged: keepCursorInWheel(mouse, wheelArea,  wheel)
			onPressed: keepCursorInWheel(mouse, wheelArea, wheel)

			onReleased:
			{
				wheel_root.state = '';
				wheel_root.accepted() ;
			}
		}
	}

}
