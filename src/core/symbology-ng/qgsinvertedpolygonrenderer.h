/***************************************************************************
    qgsinvertedpolygonrenderer.h
    ---------------------
    begin                : April 2014
    copyright            : (C) 2014 Hugo Mercier / Oslandia
    email                : hugo dot mercier at oslandia dot com
 ***************************************************************************
 *                                                                         *
 *   This program is free software; you can redistribute it and/or modify  *
 *   it under the terms of the GNU General Public License as published by  *
 *   the Free Software Foundation; either version 2 of the License, or     *
 *   (at your option) any later version.                                   *
 *                                                                         *
 ***************************************************************************/
#ifndef QGSINVERTEDPOLYGONRENDERER_H
#define QGSINVERTEDPOLYGONRENDERER_H

#include "qgis.h"
#include "qgsrendererv2.h"
#include "qgssymbolv2.h"
#include "qgsexpression.h"
#include "qgsfeature.h"
#include "qgsgeometry.h"
#include <QScopedPointer>

/**
 * QgsInvertedPolygonRenderer is a polygon-only feature renderer used to
 * display features inverted, where the exterior is turned to an interior
 * and where the exterior theoretically spans the entire plane, allowing
 * to mask the surroundings of some features.
 *
 * It is designed on top of another feature renderer, which is called "embedded"
 * Most of the methods are then only proxies to the embedded renderer.
 * 
 * Features are collected to form one "inverted" polygon
 * during renderFeature() and rendered on stopRender().
 */
class CORE_EXPORT QgsInvertedPolygonRenderer : public QgsFeatureRendererV2
{
  public:

  /** Constructor
   * @param embeddedRenderer optional embeddedRenderer. If null, a default one will be assigned
   */
  QgsInvertedPolygonRenderer( const QgsFeatureRendererV2* embeddedRenderer = 0 );
  virtual ~QgsInvertedPolygonRenderer();

  /** Used to clone this feature renderer.*/
  virtual QgsFeatureRendererV2* clone();

  virtual void startRender( QgsRenderContext& context, const QgsFields& fields );

  /** Renders a given feature.
   * This will here collect features. The actual rendering will be postponed to stopRender()
   * @param feature the feature to render
   * @param context the rendering context
   * @param layer the symbol layer to render, if that makes sense
   * @param selected whether this feature has been selected (this will add decorations)
   * @param drawVertexMarker whether this feature has vertex markers (in edit mode usually)
   * @returns true if the rendering was ok
   */
  virtual bool renderFeature( QgsFeature& feature, QgsRenderContext& context, int layer = -1, bool selected = false, bool drawVertexMarker = false );

  /**
   * The actual rendering will take place here.
   * Features collected during renderFeature() are rendered using the embedded feature renderer
   */
  virtual void stopRender( QgsRenderContext& context );

  /** @returns a textual reprensation of the renderer */
  virtual QString dump() const;

  /** Proxy that will call this method on the embedded renderer. */
  virtual QList<QString> usedAttributes();
  /** Proxy that will call this method on the embedded renderer. */
  virtual int capabilities();
  /** Proxy that will call this method on the embedded renderer. */
  virtual QgsSymbolV2List symbols();
  /** Proxy that will call this method on the embedded renderer. */
  virtual QgsSymbolV2* symbolForFeature( QgsFeature& feature );
  /** Proxy that will call this method on the embedded renderer. */
  virtual QgsSymbolV2List symbolsForFeature( QgsFeature& feat );
  /** Proxy that will call this method on the embedded renderer. */
  virtual QgsLegendSymbologyList legendSymbologyItems( QSize iconSize );
  /** Proxy that will call this method on the embedded renderer. */
  virtual QgsLegendSymbolList legendSymbolItems( double scaleDenominator = -1, QString rule = "" );
  /** Proxy that will call this method on the embedded renderer. */
  virtual bool willRenderFeature( QgsFeature& feat );

  /** Creates a renderer out of an XML, for loading*/
  static QgsFeatureRendererV2* create( QDomElement& element );

  /** Creates an XML representation of the renderer. Used for saving purpose
   * @param doc the XML document where to create the XML subtree
   * @returns the created XML subtree
   */
  virtual QDomElement save( QDomDocument& doc );

  /** sets the embedded renderer
   * @param subRenderer the embedded renderer (will be cloned)
   */
  void setEmbeddedRenderer( const QgsFeatureRendererV2* subRenderer );
  /** @returns the current embedded renderer
   */
  const QgsFeatureRendererV2* embeddedRenderer() const;

 private:
  /** Private copy constructor. @see clone() */
  QgsInvertedPolygonRenderer( const QgsInvertedPolygonRenderer& );
  /** Private assignment operator. @see clone() */
  QgsInvertedPolygonRenderer& operator=( const QgsInvertedPolygonRenderer& );

  /** Embedded renderer */
  QScopedPointer<QgsFeatureRendererV2> mSubRenderer;

  /** Structure where the reversed geometry is built during renderFeature */
  struct CombinedFeature
  {
    QgsMultiPolygon multiPolygon; //< the final combined geometry
    QgsFeature feature;           //< one feature (for attriute-based rendering)
  };
  typedef QMap< QByteArray, CombinedFeature > FeatureCategoryMap;
  /** where features are stored, based on their symbol category */
  FeatureCategoryMap mFeaturesCategoryMap;

  /** the polygon used as exterior ring that covers the current extent */
  QgsPolygon mExtentPolygon;

  /** the current coordinate transform (or null) */
  const QgsCoordinateTransform* mTransform;

  /** fields of each feature*/
  QgsFields mFields;

  /** Class used to represent features that must be rendered
      with decorations (selection, vertex markers)
  */
  struct FeatureDecoration
  {
    QgsFeature feature;
    bool selected;
    bool drawMarkers;
    int layer;
  FeatureDecoration( QgsFeature& a_feature, bool a_selected, bool a_drawMarkers, int a_layer ) :
    feature(a_feature),selected(a_selected), drawMarkers(a_drawMarkers), layer(a_layer) {}
  };
  QList<FeatureDecoration> mFeatureDecorations;
};


#endif // QGSMASKRENDERERV2_H