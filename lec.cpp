/*
Largest Empty Circle in OpenCV.
Author: Alessandro Gentilini

Warning: algorithm is incomplete because it does not consider the candidate centers
that are the intersections between the Voronoi's edges and the convex hull.
*/

#include <opencv2/imgproc/imgproc.hpp>
#include <opencv2/highgui/highgui.hpp>
#include <iostream>

using namespace cv;
using namespace std;

static void locate_point( Mat &img, Subdiv2D &subdiv, Point2f fp, Scalar active_color )
{
    int e0 = 0, vertex = 0;

    subdiv.locate(fp, e0, vertex);

    if ( e0 > 0 )
    {
        int e = e0;
        do
        {
            Point2f org, dst;
            if ( subdiv.edgeOrg(e, &org) > 0 && subdiv.edgeDst(e, &dst) > 0 )
            {
            }

            e = subdiv.getEdge(e, Subdiv2D::NEXT_AROUND_LEFT);
        }
        while ( e != e0 );
    }
}

static float distance( const Point2f &a, const Point2f &b )
{
    return sqrt((a.x - b.x) * (a.x - b.x) + (a.y - b.y) * (a.y - b.y));
}

static void paint_voronoi( Mat &img, Subdiv2D &subdiv, const std::vector<Point2f> &ch )
{
    vector<vector<Point2f> > facets;
    vector<Point2f> centers;
    subdiv.getVoronoiFacetList(vector<int>(), facets, centers);

    double lec_radius = 0;
    Point2f lec_center;
    Point2f nearest_site;
    for ( size_t i = 0; i < facets.size(); i++ )
    {
        for ( size_t j = 0; j < facets[i].size(); j++ )
        {
            if ( pointPolygonTest(ch, facets[i][j], false) == +1 )
            {

                for ( size_t k = 0; k < centers.size(); k++ )
                {
                    float d = distance(centers[i], facets[i][j]);
                    if ( d > lec_radius )
                    {
                        lec_radius = d;
                        lec_center = facets[i][j];
                        nearest_site = centers[i];
                    }
                }
            }
        }
    }

    circle(img, lec_center, 3, Scalar(0, 0, 255));
    circle(img, lec_center, lec_radius, Scalar(0, 0, 255));
    line(img, lec_center, nearest_site, Scalar(0, 0, 255));

    vector<Point> ifacet;
    vector<vector<Point> > ifacets(1);

    for ( size_t i = 0; i < facets.size(); i++ )
    {
        ifacet.resize(facets[i].size());
        for ( size_t j = 0; j < facets[i].size(); j++ )
            ifacet[j] = facets[i][j];

        Scalar color;
        color[0] = rand() & 255;
        color[1] = rand() & 255;
        color[2] = rand() & 255;

        ifacets[0] = ifacet;
        polylines(img, ifacets, true, Scalar(255, 255, 255), 1, CV_AA, 0);
        circle(img, centers[i], 3, Scalar(255, 255, 255), -1, CV_AA, 0);
    }
}

static void paint_convexHull( Mat &img, const std::vector< Point2f > &ch )
{
    Scalar color;
    color[0] = 255;
    color[1] = 255;
    color[2] = 255;
    for ( size_t i = 0; i < ch.size(); i++ )
    {
        line(img, ch[i], ch[(i + 1) % ch.size()], color, 1, CV_AA, 0);
    }
}


int main( int, char ** )
{
    Scalar active_facet_color(0, 0, 255), delaunay_color(255, 255, 255);
    Rect rect(0, 0, 600, 600);

    Subdiv2D subdiv(rect);
    Mat img(rect.size(), CV_8UC3);

    img = Scalar::all(0);
    string win = "Largest empty circle";
    imshow(win, img);

    std::vector< Point2f > points;
    srand(2013);
    for ( int i = 0; i < 360; i += 1 )
    {
        Point2f fp( (float)( rand() % 35 + rect.width / 4 * cos(2 * 3.1415 * float(i) / 360) + rect.width / 2 ),
                    (float)( rand() % 35 + rect.height / 4 * sin(2 * 3.1415 * float(i) / 360) + rect.height / 2) );

        locate_point( img, subdiv, fp, active_facet_color );
        subdiv.insert(fp);
        points.push_back(fp);
        circle(img, fp, 7, Scalar(0, 255, 0));
    }

    std::vector< Point2f > ch;
    convexHull(points, ch);
    paint_convexHull(img, ch);

    paint_voronoi( img, subdiv, ch );

    imshow( win, img );

    waitKey(0);

    return 0;
}
