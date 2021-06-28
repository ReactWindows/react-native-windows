/**
 * Copyright (c) Microsoft Corporation.
 * Licensed under the MIT License.
 * @format
 */

import React from 'react';
import {Button, Picker, Switch, Text, TextInput, View} from 'react-native';
import {Flyout, Popup, Placement} from 'react-native-windows';

interface IFlyoutExampleState {
  isFlyoutVisible: boolean;
  isFlyoutTwoVisible: boolean;
  isPopupVisible: boolean;
  buttonTitle: string;
  isLightDismissEnabled: boolean;
  isOverlayEnabled: boolean;
  popupSwitchState: boolean;
  placementOptions: Placement;
}

const placementValues: string[] = [
  'top',
  'bottom',
  'left',
  'right',
  'full',
  'top-edge-aligned-left',
  'top-edge-aligned-right',
  'bottom-edge-aligned-left',
  'bottom-edge-aligned-right',
  'left-edge-aligned-top',
  'right-edge-aligned-top',
  'left-edge-aligned-bottom',
  'right-edge-aligned-bottom',
];

class FlyoutExample extends React.Component<{}, IFlyoutExampleState> {
  private _anchor: any;
  private _anchorTwo: any;

  public state: IFlyoutExampleState = {
    isFlyoutVisible: false,
    isFlyoutTwoVisible: false,
    isPopupVisible: false,
    buttonTitle: 'Open Flyout',
    isLightDismissEnabled: true,
    isOverlayEnabled: false,
    popupSwitchState: true,
    placementOptions: 'top',
  };

  public constructor(props: any) {
    super(props);
    this._anchor = React.createRef();
    this._anchorTwo = React.createRef();
  }

  public render() {
    const lorumIpsum =
      'Lorem ipsum dolor sit amet, consectetur adipiscing elit. Sed dapibus felis eget augue condimentum suscipit. Suspendisse hendrerit, libero aliquet malesuada tempor, urna nibh consectetur tellus, vitae efficitur quam erat non mi. Maecenas vitae eros sit amet quam vestibulum porta sed sit amet tellus. Fusce quis lectus congue, fringilla arcu id, luctus urna. Cras sagittis ornare mauris sit amet dictum. Vestibulum feugiat laoreet fringilla. Vivamus ac diam vehicula felis venenatis sagittis vitae ultrices elit. Curabitur libero augue, laoreet quis orci vitae, congue euismod massa. Aenean nec odio sed urna vehicula fermentum non a magna. Quisque ut commodo neque, eget eleifend odio. Sed sit amet lacinia sem. Suspendisse in metus in purus scelerisque vestibulum. Nam metus dui, efficitur nec metus non, tincidunt pharetra sapien. Praesent id convallis metus, ut malesuada arcu. Quisque quam libero, pharetra eu tellus ac, aliquam fringilla erat. Quisque tempus in lorem ac suscipit.';

    return (
      <View>
        <View style={{flexDirection: 'row', paddingTop: 20}}>
          <Text style={{padding: 10}}>Placement Options: </Text>
          <Picker
            style={{width: 200, height: 35}}
            selectedValue={this.state.placementOptions}
            onValueChange={value => this.setState({placementOptions: value})}>
            {placementValues.map(item => (
              <Picker.Item key={item} label={item} value={item} />
            ))}
          </Picker>
        </View>
        <View style={{justifyContent: 'center', padding: 20, width: 200}}>
          <Button onPress={this._onPress} title={this.state.buttonTitle} />
        </View>
        <View style={{flexDirection: 'row', paddingTop: 200}}>
          <Text style={{padding: 10, width: 300, height: 32}}>
            Text Input to Anchor flyout to:{' '}
          </Text>
          <TextInput style={{height: 32, width: 250}} ref={this._setRef} />
        </View>
        {this.state.isFlyoutVisible && (
          <Flyout
            isOpen={this.state.isFlyoutVisible}
            isLightDismissEnabled={this.state.isLightDismissEnabled}
            isOverlayEnabled={this.state.isOverlayEnabled}
            onDismiss={this._onFlyoutDismissed}
            target={this._anchor}
            placement={this.state.placementOptions}>
            <View
              style={{backgroundColor: 'lightgray', width: 300, height: 400}}>
              <Text
                style={{
                  justifyContent: 'center',
                  paddingTop: 10,
                  paddingBottom: 30,
                }}>
                This is a flyout
              </Text>
              <Button onPress={this._onFlyoutButtonPressed} title="Close" />
              <View style={{flexDirection: 'row'}}>
                <Text style={{padding: 10}}>isLightDismissEnabled: </Text>
                <Switch
                  style={{justifyContent: 'center', padding: 20}}
                  value={this.state.popupSwitchState}
                  onValueChange={value =>
                    this.setState({
                      popupSwitchState: value,
                      isLightDismissEnabled: value,
                    })
                  }
                />
              </View>
              <View style={{flexDirection: 'row'}}>
                <Text style={{padding: 10}}>isOverlayEnabled: </Text>
                <Switch
                  style={{justifyContent: 'center', padding: 20}}
                  value={this.state.isOverlayEnabled}
                  onValueChange={value =>
                    this.setState({
                      isOverlayEnabled: value,
                    })
                  }
                />
              </View>
              <TextInput style={{height: 32, width: 100}} />
              <Text
                style={{
                  justifyContent: 'center',
                  paddingTop: 10,
                  marginLeft: 50,
                }}>
                Placement Options
              </Text>
              <Picker
                style={{
                  borderBottomWidth: 1,
                  borderTopWidth: 1,
                  borderLeftWidth: 1,
                  borderRightWidth: 1,
                  width: 200,
                  marginLeft: 50,
                }}
                selectedValue={this.state.placementOptions}
                onValueChange={value =>
                  this.setState({placementOptions: value})
                }>
                {placementValues.map(item => (
                  <Picker.Item key={item} label={item} value={item} />
                ))}
              </Picker>
              <View
                style={{
                  width: 150,
                  marginLeft: 75,
                  marginTop: 10,
                }}>
                <Button
                  onPress={this._onPressTwo}
                  title={'Open Another Flyout'}
                  ref={this._setRefTwo}
                />
              </View>
              <View
                style={{
                  width: 150,
                  marginLeft: 75,
                  marginTop: 10,
                }}>
                <Button
                  onPress={() => {
                    this.setState({isPopupVisible: true});
                  }}
                  title={'Open A Popup'}
                />
              </View>
            </View>
          </Flyout>
        )}
        {this.state.isFlyoutTwoVisible && (
          <Flyout
            isOpen={this.state.isFlyoutTwoVisible}
            isLightDismissEnabled={true}
            onDismiss={this._onFlyoutTwoDismissed}
            target={this._anchorTwo}
            placement={this.state.placementOptions}>
            <View
              style={{backgroundColor: 'lightblue', width: 200, height: 300}}>
              <Text>{lorumIpsum}</Text>
            </View>
          </Flyout>
        )}
        {this.state.isPopupVisible && (
          <Popup
            isOpen={this.state.isPopupVisible}
            isLightDismissEnabled={this.state.isLightDismissEnabled}
            target={this._anchorTwo}
            onDismiss={() => {
              this.setState({isPopupVisible: false});
            }}>
            <View
              style={{backgroundColor: 'lightblue', width: 200, height: 300}}>
              <Text>{lorumIpsum}</Text>
              <Button
                onPress={() => {
                  this.setState({isPopupVisible: false});
                }}
                title="Close"
              />
            </View>
          </Popup>
        )}
      </View>
    );
  }

  private readonly _setRef = (textInput: TextInput) => {
    this._anchor = textInput;
  };

  _onPress = () => {
    this.setState({buttonTitle: 'Close Flyout', isFlyoutVisible: true});
  };

  _onFlyoutButtonPressed = () => {
    this.setState({buttonTitle: 'Open Flyout', isFlyoutVisible: false});
  };

  _onFlyoutDismissed = (_isOpen: boolean) => {
    this.setState({isFlyoutVisible: false});
    this.setState({isFlyoutTwoVisible: false});
    this.setState({isPopupVisible: false});
    this.setState({buttonTitle: 'Open Flyout'});
  };

  _setRefTwo = (element: any) => {
    this._anchorTwo = element;
  };

  _onPressTwo = () => {
    this.setState({isFlyoutTwoVisible: true});
  };

  _onFlyoutTwoDismissed = (_isOpen: boolean) => {
    this.setState({isFlyoutTwoVisible: false});
  };
}

export const displayName = (_undefined?: string) => {};
export const title = 'Flyout';
export const category = 'Basic';
export const description =
  'Displays content on top of existing content, within the bounds of the application window.';
export const examples = [
  {
    title: 'Flyout Anchor to text input',
    render: function(): JSX.Element {
      return <FlyoutExample />;
    },
  },
];
