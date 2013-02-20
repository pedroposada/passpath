<?php

/****************************************************************************************************
 *              ADMIN (form, validate, submit and ajax)
 ****************************************************************************************************/
function passpath_admin_form($form, &$form_state) {
  $form['#theme'] = 'passpath_admin_form';
  $form['#tree'] = TRUE;
  $form['#prefix'] = '<div id="passpath-form-wrapper">';
  $form['#suffix'] = '</div>';
  $tabsvals = variable_get('passpath_admin_form_values');
  // get current number of headlines
  $num = isset($tabsvals) ? count($tabsvals) : 1;
  if (isset($form_state['storage']['extraheadlines'])) {
    $num += (int)($form_state['storage']['extraheadlines']);
  }
  // add items
  $i = 0;
  do {
    $element["path"] = array(
      '#type' => 'textfield',
      '#default_value' => isset($tabsvals[$i]) ? $tabsvals[$i]['path'] : '',
      '#description' => t('An internal drupal path.'),
    );
    $element["pass"] = array(
      '#type' => 'textfield',
      '#default_value' => isset($tabsvals[$i]) ? $tabsvals[$i]['pass'] : '',
      '#description' => t('Passsword to access this path.'),
      '#maxlength' => 600,
    );
    $element["expire"] = array(
      '#type' => 'textfield',
      '#default_value' => isset($tabsvals[$i]) ? $tabsvals[$i]['expire'] : '',
      '#description' => t('Session duration in hours.'),
    );
    $element["weight"] = array(
      '#type' => 'weight',
      '#title' => t('Weight'),
      '#delta' => 10,
      '#title-display' => 'invisible',
      '#default_value' => isset($tabsvals[$i]) ? $tabsvals[$i]['weight'] : $i,
    );
    $element["remove"] = array(
      '#type' => 'submit',
      '#value' => t('Remove'),
      '#ajax' => array(
        'callback' => 'passpath_admin_form_ajax',
        'wrapper' => 'passpath-form-wrapper',
      ),
      '#name' => "remove_$i"
    );
    $form['tabs'][$i] = $element;   
    $i++;
  } while($i < $num);
  // bottom buttons
  $form['add'] = array(
    '#type'=>'submit',
    '#value'=>t('Add more'),
    '#ajax' => array(
      'callback' => 'passpath_admin_form_ajax',
      'wrapper' => 'passpath-form-wrapper',
    ),
    '#name' => 'add',
    '#weight' => 100,
  );
  $form['save'] = array(
    '#type'=>'submit',
    '#value'=>t('Save'),
    '#ajax' => array(
      'callback' => 'passpath_admin_form_ajax',
      'wrapper' => 'passpath-form-wrapper',
    ),
    '#name' => 'save',
    '#weight' => 102,
  );
  return $form;
}
function passpath_admin_form_validate($form, &$form_state) {
  // remove
  if (strstr($form_state['triggering_element']['#name'],'remove_')) {
    if (count($_POST['tabs']) < 2) {
      form_set_error('',"Cannot remove this item. The minimum number of items in this list is 1.");
    }
  }
  // preview, save
  if (in_array($form_state['triggering_element']['#name'],array('preview','save'))) {
    $keys = array();
    foreach ($form_state['values']['tabs'] as $key => $value) {
      if (!in_array($key, $form_state['storage']['remove'])) {
        if (empty($value['path'])) {
          form_error($form['tabs'][$key]['tabtitle'], "Path is required.");
        }
        if (!valid_url($value['path'])) {
          form_error($form['tabs'][$key]['path'], "Invalid path.");
        }
        if (empty($value['pass'])) {
          form_error($form['tabs'][$key]['pass'], "Password is required.");
        }
        if (!ctype_digit($value['expire'])) {
          form_error($form['tabs'][$key]['expire'], "Expiration must be a number of hours.");
        }
      }
    }
  }
}
function passpath_admin_form_submit($form, &$form_state) {
  // add
  if ($form_state['triggering_element']['#name'] == 'add') {
    $form_state['storage']['extraheadlines'] += 1;
  }
  // remove
  if (strstr($form_state['triggering_element']['#name'],'remove_')) {
    $removed = $form_state['triggering_element']['#array_parents'][1];
    $form_state['storage']['remove'][] = $removed;
    drupal_set_message(t("Changes made in this table will not be stored until the form is Saved."),'warning');
  }
  // preview, save
  if (in_array($form_state['triggering_element']['#name'],array('preview','save'))) {
    $weights = $values = array();
    foreach ($form_state['values']['tabs'] as $key => $value) {
      if (!in_array($key, $form_state['storage']['remove'])) {
        $values[] = $value;
        $weights[] = $value['weight'];
      }
    }
    array_multisort($weights, $values);
    $form_state['storage']['preview_pane'] = theme('gs_helper_lgbt_external_links',array('items'=>$values));
  }
  // save
  if ($form_state['triggering_element']['#name'] == 'save') {
    variable_set('passpath_admin_form_values', $values);
    drupal_set_message("Values were saved successfully.");
    unset($form_state['storage']['extraheadlines']);
  }
  // maintain state of form
  $form_state['rebuild'] = TRUE;
}
function passpath_admin_form_ajax($form, $form_state) {
  // The form has already been submitted and updated.
  // remove
  foreach (element_children($form['tabs']) as $key) {
    if (isset($form_state['storage']['remove']) && in_array($key, $form_state['storage']['remove'])) {
      unset($form['tabs'][$key]);
    }
  }
  return $form;
}