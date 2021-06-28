/**
 * Copyright (c) Microsoft Corporation.
 * Licensed under the MIT License.
 *
 * @format
 */

import React from 'react';
import RN from 'react-native';
import {useEffect, useRef} from 'react';
import {assert} from 'chai';
import {componentTest, TestComponentType} from './lib/TestDefinition';

function onNativeRender(callback: () => void) {
  // We need to wait until native has rendered a frame before measuring will
  // return non-zero results. Use RAF to schedule work on the next render, to
  // then shceduled work on the render after (at which point we should be all
  // good).
  requestAnimationFrame(() => {
    requestAnimationFrame(() => {
      callback();
    });
  });
}

function mountAndMeasure(
  Component: React.ComponentType<any>,
): TestComponentType {
  return ({pass}) => {
    const ref = useRef<React.Component<any> & Readonly<RN.NativeMethods>>(null);
    useEffect(() => {
      onNativeRender(() => {
        // Not all components offer forwarded refs to native components. Only
        // try to measure those who do. Typings aren't great here...
        // eslint-disable-next-line @typescript-eslint/no-unnecessary-condition
        if (!ref.current!.measure) {
          pass();
          return;
        }

        ref.current!.measure((_x, _y, width, height) => {
          assert.isAtLeast(
            width,
            1,
            'Component should have a measured width of at least 1',
          );
          assert.isAtLeast(
            height,
            1,
            'Component should have a measured height of at least 1',
          );
          pass();
        });
      });
    });

    return <Component ref={ref} />;
  };
}

componentTest('ActivityIndicator', mountAndMeasure(RN.ActivityIndicator));

componentTest(
  'Button',
  mountAndMeasure(
    React.forwardRef<RN.Button>((_, ref) => (
      <RN.Button ref={ref} title="Test" onPress={() => {}} />
    )),
  ),
);

componentTest(
  'Image',
  mountAndMeasure(
    React.forwardRef<RN.Image>((_, ref) => (
      <RN.Image
        ref={ref}
        source={require('react-native-windows/IntegrationTests/blue_square.png')}
      />
    )),
  ),
);

componentTest(
  'KeyboardAvoidingView',
  mountAndMeasure(
    React.forwardRef<RN.KeyboardAvoidingView>((_, ref) => (
      <RN.KeyboardAvoidingView ref={ref}>
        <RN.View style={{height: 25, width: 25, backgroundColor: 'orange'}} />
      </RN.KeyboardAvoidingView>
    )),
  ),
);

// Need a real implementaion here once we supoprt Modal
componentTest.skip('Modal', mountAndMeasure(RN.Modal));

componentTest('Picker', props => {
  const PickerTestComponent = mountAndMeasure(
    React.forwardRef<RN.Image>((_, ref) => (
      <RN.Picker ref={ref}>
        <RN.Picker.Item label="foo" value="bar" />
      </RN.Picker>
    )),
  );

  return <PickerTestComponent {...props} />;
});

componentTest(
  'Pressable',
  mountAndMeasure(
    React.forwardRef<typeof RN.Pressable>((_, ref) => (
      // @ts-ignore Pressable typings are bad
      <RN.Pressable ref={ref}>
        <RN.View style={{height: 25, width: 25, backgroundColor: 'orange'}} />
      </RN.Pressable>
    )),
  ),
);

componentTest(
  'SafeAreaView',
  mountAndMeasure(
    React.forwardRef<RN.SafeAreaView>((_, ref) => (
      <RN.SafeAreaView ref={ref}>
        <RN.View style={{height: 25, width: 25, backgroundColor: 'orange'}} />
      </RN.SafeAreaView>
    )),
  ),
);

componentTest(
  'ScrollView',
  mountAndMeasure(
    React.forwardRef<RN.ScrollView>((_, ref) => (
      <RN.ScrollView ref={ref}>
        <RN.View style={{height: 25, width: 25, backgroundColor: 'orange'}} />
      </RN.ScrollView>
    )),
  ),
);

// Slider needs a height set to render (#5437)
componentTest.skip('Slider', mountAndMeasure(RN.Slider));

componentTest('Switch', mountAndMeasure(RN.Switch));

componentTest(
  'Text',
  mountAndMeasure(
    React.forwardRef<RN.Text>((_, ref) => <RN.Text ref={ref}>Hello</RN.Text>),
  ),
);

componentTest('TextInput', mountAndMeasure(RN.TextInput));

componentTest(
  'TouchableHighlight',
  mountAndMeasure(
    React.forwardRef<RN.TouchableHighlight>((_, ref) => (
      <RN.TouchableHighlight ref={ref}>
        <RN.View style={{height: 25, width: 25, backgroundColor: 'orange'}} />
      </RN.TouchableHighlight>
    )),
  ),
);

componentTest(
  'TouchableOpacity',
  mountAndMeasure(
    React.forwardRef<RN.TouchableOpacity>((_, ref) => (
      <RN.TouchableOpacity ref={ref}>
        <RN.View style={{height: 25, width: 25, backgroundColor: 'orange'}} />
      </RN.TouchableOpacity>
    )),
  ),
);

componentTest(
  'TouchableWithoutFeedback',
  mountAndMeasure(
    React.forwardRef<RN.TouchableWithoutFeedback>((_, ref) => (
      <RN.TouchableWithoutFeedback ref={ref}>
        <RN.View style={{height: 25, width: 25, backgroundColor: 'orange'}} />
      </RN.TouchableWithoutFeedback>
    )),
  ),
);

componentTest(
  'View',
  mountAndMeasure(
    React.forwardRef<RN.View>((_, ref) => (
      <RN.View
        ref={ref}
        style={{height: 50, width: 50, backgroundColor: 'green'}}
      />
    )),
  ),
);
