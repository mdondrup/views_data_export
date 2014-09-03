<?php
/**
 * Webform exporter for creating XML files.
 */
class ViewsDataExportExporterXML extends ViewsDataExportExporter implements ViewsDataExportExporterUserConfigurationInterface
{
  private $exporter_options = array();

  /**
   * Constructor for views_data_export_exporter classes.
   *
   * @param $options
   *   The array of export options as provided by the user-interface.
   */
  function __construct($options) {
    $this->options = $options;
  }


  /**
   * Set options fields and default values.
   *
   * @return
   * An array of options information.
   */
  function option_definition() {
    //$options = parent::option_definition();

    $options['transform'] = array(
      'default' => TRUE,
      'translatable' => FALSE,
    );
    $options['transform_type'] = array(
      'default' => 'dash',
      'translatable' => FALSE,
    );
    $options['root_node'] = array(
      'default' => '',
      'translatable' => FALSE,
    );
    $options['item_node'] = array(
      'default' => '',
      'translatable' => FALSE,
    );
    $options['no_entity_encode'] = array(
      'default' => array(),
      'translatable' => FALSE,
    );
    $options['cdata_wrapper'] = array(
      'default' => array(),
      'translatable' => FALSE,
    );

    return $options;
  }

  /**
   * Options form mini callback.
   *
   * @param $form
   * Form array to add additional fields to.
   * @param $form_state
   * State of the form.
   * @return
   * None.
   */
  function options_form(&$form, &$form_state, $field_labels) {
    //$options = parent::options_form($form, $form_state);

    $form['transform'] = array(
      '#type' => 'checkbox',
      '#title' => t('Transform spaces'),
      '#default_value' => $this->options['transform'],
      '#description' => t('Transform spaces to valid XML in field labels (spaces create invalid XML markup). Note that invalid XML tag characters will always be converted.'),
    );
    $form['transform_type'] = array(
      '#type' => 'select',
      '#title' => t('Transform type'),
      '#default_value' => $this->options['transform_type'],
      '#options' => array(
        'dash' => t('Dash'),
        'underline' => t('Underline'),
        'camel' => t('camelCase'),
        'pascal' => t('PascalCase'),
      ),
      '#process' => array('ctools_dependent_process'),
      '#dependency' => array(
        'edit-style-options-transform' => array(TRUE),
      ),
    );
    $form['root_node'] = array(
      '#type' => 'textfield',
      '#title' => t('Root node'),
      '#default_value' => $this->options['root_node'],
      '#description' => t('The XML tag for the root node.'),
    );
    $form['item_node'] = array(
      '#type' => 'textfield',
      '#title' => t('Item node'),
      '#default_value' => $this->options['item_node'],
      '#description' => t('The XML tag for an item node.'),
    );

    if (!empty($field_labels)) {
      // changed to $options['field_labels'] from $options
      if (empty($options['no_entity_encode'])) {
        $options['no_entity_encode'] = array();
      }

      $form['no_entity_encode'] = array(
        '#type' => 'checkboxes',
        '#title' => t('Disable encoding of XML entities for these fields'),
        '#options' => $field_labels,
        '#default_value' =>$this->options['no_entity_encode'],
        '#description' => t('If checked field contents will be outputted '.
          '<em>without encoding</em> of XML entities. This is '.
          'useful when when used in conjunction with a field '.
          'formatter that outputs properly formatted and '.
          'encoded XML data.'),
      );

      if (empty($options['cdata_wrapper'])) {
        $options['cdata_wrapper'] = array();
      }

      $form['cdata_wrapper'] = array(
        '#type' => 'checkboxes',
        '#title' => t('Field values to wrap using CDATA'),
        '#options' => $field_labels,
        '#default_value' => $this->options['cdata_wrapper'],
        '#description' => t('If checked the fields content will be wrapped using the CDATA tag.'),
      );
    }
  }

  /**
   * Perform any necessary changes to the form values prior to storage.
   * There is no need for this function to actually store the data.
   *
   * @param $form
   * @param $form_state
   */
  function options_submit(&$form, &$form_state) {
    if (isset($form_state['values']['style_options']['no_entity_encode'])) {
      // Remove any options values set to 0
      $form_state['values']['style_options']['no_entity_encode'] = array_filter($form_state['values']['style_options']['no_entity_encode']);
    }
    if (isset($form_state['values']['style_options']['cdata_wrapper'])) {
      // Remove any options values set to 0
      $form_state['values']['style_options']['cdata_wrapper'] = array_filter($form_state['values']['style_options']['cdata_wrapper']);
    }
  }

  /**
   * Add a single row to the export file.
   *
   * @param $file_handle
   *   A PHP file handle to the export file.
   * @param array $data
   *   An array of formatted data for this row. One cell per item.
   * @param int $row_count
   *   The current number of rows in the export file.
   */
  function add_row(&$file_handle, $data, $row_count) {
  }

  /**
   * Provide headers to the page when an export file is being downloaded.
   *
   * @param $filename
   *   The name of the file being downloaded. e.g. export.xls.
   */
  function get_headers($filename) {
    $headers = array();
    $headers['Content-Type'] = 'application/force-download';
    $headers['Pragma'] = 'public';
    $headers['Cache-Control'] = 'max-age=0';
    return $headers;
  }

  /**
   * Write the start of the export file.
   *
   * @param $file_handle
   *   A PHP file handle to the export file.
   */
  function bof(&$file_handle) {
  }

  /**
   * Write the end of the export file.
   *
   * @param $file_handle
   *   A PHP file handle to the export file.
   */
  function eof(&$file_handle, $row_count, $col_count) {
  }

  /**
   * Allow final processing of the results.
   *
   * @param $results
   *   An array of result data, including:
   *   - file_name: The full file path of the generated file.
   *   - row_count: The final number of rows in the generated file.
   */
  function post_process(&$results) {
  }

  /**
   * Validate the options.
   */
  function options_validate(&$form, &$form_state) {

  }
}
